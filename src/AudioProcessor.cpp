//
// Created by chrtopf on 07.06.23.
//
#include <sstream>
#include <cstring>
#include "AudioProcessor.h"
#include "Log.h"

int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData){
    //first, check if the last bunch of samples was already processed
    if(!AudioProcessor::sampleBuffer.empty()){
        //skip the next couple of samples
        Log::d("Too slow, skipping samples");
        return paContinue;
    }
    //cast the input buffer
    const auto* samples = static_cast<const float*>(inputBuffer);

    //fill the sample buffer again
    for (unsigned int i = 0; i < framesPerBuffer; ++i)
    {
        AudioProcessor::sampleBuffer.push_back(samples[i]);
    }

    //notify the audio _processor about a new set of samples
    {
        std::lock_guard<std::mutex> lock(mutex);
        AudioProcessor::bufferReady = true;
    }
    AudioProcessor::conditionVariable.notify_one();

    //return the parameter to continue the stream
    return paContinue;
}

void runProcessThread(){
    AudioProcessor::getInstance()->onProcessSamples();
}

AudioProcessor::AudioProcessor() {
    //initialize port audio
    PaError error = Pa_Initialize();
    if(error != paNoError){
        printPaError("Could not initialize PortAudio.", error);
        exit(1);
    }
    //set the default stream index
    _audioStreamIndex = Pa_GetDefaultOutputDevice();
}

AudioProcessor *AudioProcessor::getInstance() {
    if(AudioProcessor::instance == nullptr) {
        return new AudioProcessor();
    }
    return instance;
}

void AudioProcessor::destroy() {
    delete AudioProcessor::instance;
}

void AudioProcessor::printPaError(const std::string &text, const PaError &error) {
    stringstream ss;
    ss << text << " Error: " << Pa_GetErrorText(error);
    Log::e(ss.str());
}

//region effects

void AudioProcessor::setEffect(Effect *effect) {
    std::lock_guard<std::mutex> lock(effectMutex);
    //do not overwrite existing effects, as this could create a memory leak
    if(_effect == nullptr){
        _effect = effect;
    }else{
        delete _effect;
        _effect = effect;
    }
}

bool AudioProcessor::hasEffect() {
    return _effect != nullptr;
}

//endregion

//region core

bool AudioProcessor::start() {
    //start the thread here
    {
        std::lock_guard<std::mutex> lock(interruptMutex);
        shouldRun = true;
    }
    processor = std::thread(runProcessThread);
    //check if the correct device was found
    if(_audioStreamIndex < 0 || (_audioStreamIndex + 1) > Pa_GetDeviceCount()){
        //chose the default output device instead
        _audioStreamIndex = Pa_GetDefaultOutputDevice();
        //report the issue
        stringstream ss;
        ss << "Could not find the audio stream with index " << _audioStreamIndex << ". Using the default stream " << Pa_GetDefaultOutputDevice() << " '" << Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->name << "' now.";
        Log::w(ss.str());
    }else{
        //print the chosen stream
        stringstream ss;
        ss << "Using the audio stream '" << Pa_GetDeviceInfo(_audioStreamIndex)->name << "' (" << _audioStreamIndex << ") now.";
        Log::i(ss.str());
    }
    //configure the input device
    PaStreamParameters inputParameters;
    inputParameters.device = _audioStreamIndex;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    //get and the default sample rate for the device stream
    double sampleRate = Pa_GetDeviceInfo(_audioStreamIndex)->defaultSampleRate;
    stringstream ss;
    ss << "Reading with sample rate " << sampleRate << ".";
    Log::i(ss.str());
    //open the device for recording
    PaError error = Pa_OpenStream(&stream, &inputParameters, NULL, sampleRate, BUFFER_SIZE, paClipOff, audioCallback, NULL);
    if(error != paNoError){
        printPaError("Could not open audio stream.", error);
        return false;
    }
    //start the audio stream
    error = Pa_StartStream(stream);
    if(error != paNoError){
        printPaError("Could not start audio stream.", error);
        return false;
    }
    return true;
}

void AudioProcessor::onProcessSamples() {
    bool running = true;
    while (running){
        //wait for new samples
        {
            std::unique_lock<std::mutex> lock(audioBufferMutex);
            conditionVariable.wait(lock, []{return bufferReady;});
        }
        bufferReady = false;

        if(!sampleBuffer.empty()){
            {
                std::lock_guard<std::mutex> lock(effectMutex);
                //check if a _processor is available
                if(_effect != nullptr){
                    //then process the samples
                    _effect->onData(sampleBuffer);
                }
            }
            sampleBuffer.clear();
        }

        //check if this thread needs to be stopped
        {
            std::lock_guard<std::mutex> lock(interruptMutex);
            running = shouldRun;
        }
    }
    Log::d("Stopped sample processing thread.");
}

void AudioProcessor::stop() {
    //stop the processing thread
    {
        std::lock_guard<std::mutex> lock(interruptMutex);
        shouldRun = false;
    }
    //notify the audio _processor about a new set of samples although there are none to prevent a deadlock
    {
        std::lock_guard<std::mutex> lock(mutex);
        AudioProcessor::bufferReady = true;
    }
    AudioProcessor::conditionVariable.notify_one();
    //then wait for the thread to finish
    processor.join();
    //stop the audio stream
    PaError error = Pa_StopStream(stream);
    if(error != paNoError){
        printPaError("Could not stop audio stream.", error);
    }
    //close the stream
    error = Pa_CloseStream(stream);
    if(error != paNoError){
        printPaError("Could not close audio stream.", error);
    }
}

//endregion

vector<string> AudioProcessor::printAudioStreams(const vector<string> &blackList) {
    //get the connected devices
    int connectedDevices = Pa_GetDeviceCount();
    if(connectedDevices < 0){
        Log::e("Could not determine the amount of audio devices.");
    }
    vector<string> streamNames;
    streamNames.resize(connectedDevices);
    //iterate through all available devices
    for (int i = 0; i < connectedDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        //check if the stream is blacklisted
        bool isBlacklisted = false;
        for(int j = 0; j < blackList.size(); j++){
            if(blackList[j].compare(deviceInfo->name) == 0){
                isBlacklisted = true;
                break;
            }
        }
        //print the stream if possible
        if (deviceInfo->maxInputChannels > 0 && !isBlacklisted) {
            //save the name of that stream
            streamNames[i] = deviceInfo->name;
            //print each stream with its index
            cout << "[" << i << "]" << " Name: '" << deviceInfo->name << "', Sample-Rate: " << deviceInfo->defaultSampleRate << endl;
        }
    }
    return streamNames;
}

bool AudioProcessor::setAudioStreamByName(string name) {
    //get the connected devices
    int connectedDevices = Pa_GetDeviceCount();
    if(connectedDevices < 0){
        Log::e("Could not determine the amount of audio devices.");
    }
    int index = -1;
    //iterate through all available devices to find the right one
    for (int i = 0; i < connectedDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0 && name.compare(deviceInfo->name) == 0) {
            //save the name of that stream
            index = i;
            break;
        }
    }
    //check if the stream was not found
    if(index < 0){
        stringstream ss;
        ss << "The audio stream with name '" << name << "' does not exist.";
        Log::e(ss.str());
        return false;
    }
    _audioStreamIndex = index;
    return true;
}
