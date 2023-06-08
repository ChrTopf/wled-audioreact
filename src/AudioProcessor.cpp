//
// Created by chrtopf on 07.06.23.
//
#include <sstream>
#include <string.h>
#include "AudioProcessor.h"
#include "Log.h"

int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData){
    //first, check if the last bunch of samples was already processed
    if(!AudioProcessor::sampleBuffer.empty()){
        //skip the next couple of samples
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

AudioProcessor *AudioProcessor::getInstance() {
    if(AudioProcessor::instance == nullptr) {
        return new AudioProcessor();
    }
    return instance;
}

void AudioProcessor::destroy() {
    delete AudioProcessor::instance;
}

void AudioProcessor::setEffect(Effect *effect) {
    _effect = effect;
}

bool AudioProcessor::hasEffect() {
    return _effect != nullptr;
}

void AudioProcessor::removeEffect() {
    //delete the effect
    delete _effect;
    _effect = nullptr;
}

bool AudioProcessor::start() {
    //start the thread here
    processor = std::thread(runProcessThread);
    //initialize port audio
    PaError error = Pa_Initialize();
    if(error != paNoError){
        printPaError("Could not initialize PortAudio.", error);
        return false;
    }
    /*//get the connected devices
    int connectedDevices = Pa_GetDeviceCount();
    if(connectedDevices < 0){
        Log::e("Could not determine the amount of audio devices.");
        return false;
    }
    //iterate through all available devices
    int deviceIndex = -1;
    for (int i = 0; i < connectedDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0 && strcmp(deviceInfo->name, "spotify") == 0) {
            deviceIndex = i;
            break;
        }
    }
    //check if the correct device was found
    if(deviceIndex < 0){
        Log::e("Could not find the system default stream.");
        return false;
    }
    cout << Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->name << endl;*/
    //configure the input device
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultOutputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    //open the device for recording
    error = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLE_RATE, BUFFER_SIZE, paClipOff, audioCallback, NULL);
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
    while (shouldRun){
        //wait for new samples
        {
            std::unique_lock<std::mutex> lock(mutex);
            conditionVariable.wait(lock, []{return bufferReady;});
        }

        if(!sampleBuffer.empty() && shouldRun){
            //check if a _processor is available
            if(_effect != nullptr){
                //then process the samples
                _effect->onData(sampleBuffer);
            }
            sampleBuffer.clear();
        }
    }
}

void AudioProcessor::stop() {
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
    //stop the processing thread
    shouldRun = false;
    processor.join();
}

void AudioProcessor::printPaError(const std::string &text, const PaError &error) {
    stringstream ss;
    ss << text << " Error: " << Pa_GetErrorText(error);
    Log::e(ss.str());
}
