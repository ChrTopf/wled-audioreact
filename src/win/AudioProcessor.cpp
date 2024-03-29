//
// Created by chrtopf on 07.06.23.
//
#include "AudioProcessor.h"

void runProcessThread(){
    AudioProcessor::getInstance()->onProcessSamples();
}

void runAudioCapture(){
    AudioProcessor::getInstance()->onRun();
}

AudioProcessor::AudioProcessor() {
    //set the default stream index and sample rate
    _audioStreamIndex = getDefaultStreamIndex();
    _sampleRate = getDeviceSampleRate(_audioStreamIndex);
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
    //start the processing thread here
    {
        std::lock_guard<std::mutex> lock(interruptProcessing);
        AudioProcessor::shouldProcess = true;
    }
    processor = std::thread(runProcessThread);

    //start the recording thread here
    {
        std::lock_guard<std::mutex> lock(interruptRecording);
        AudioProcessor::shouldRecord = true;
    }
    recorder = std::thread(runAudioCapture);
    return true;
}

void AudioProcessor::onRun() {
    //start recording
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    //get device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (!FAILED(hr)) {
        //get all devices
        IMMDeviceCollection* pDeviceCollection = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDeviceCollection);
        if(!FAILED(hr)){
            //get the device count
            UINT deviceCount = 0;
            hr = pDeviceCollection->GetCount(&deviceCount);
            if (!FAILED(hr) && deviceCount > 0) {
                //search for the selected device
                IMMDevice* device = nullptr;
                //go through all devices
                for (UINT i = 0; i < deviceCount; ++i) {
                    //get a device
                    IMMDevice* pDevice = nullptr;
                    hr = pDeviceCollection->Item(i, &pDevice);
                    if (!FAILED(hr)) {
                        //get the device id
                        LPWSTR deviceId = nullptr;
                        hr = pDevice->GetId(&deviceId);
                        if(!FAILED(hr)){
                            if(wcscmp(deviceId, _audioStreamIndex) == 0){
                                device = pDevice;
                                break;
                            }
                        }else{
                            Log::e("WASAPI: Could not get the id of a device.");
                        }
                        pDevice->Release();
                    }else{
                        Log::w("WASAPI: Could not get a device from the collection.");
                    }
                }

                //check if the device could not be found
                if(device == nullptr){
                    //get the default stream
                    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
                    if(!FAILED(hr)){
                        std::string defaultDeviceName = getDeviceName(device);
                        //set the sample rate to the default one of the device
                        _sampleRate = getDeviceSampleRate(device);
                        std::stringstream ss;
                        ss << "Could not find the audio stream with index " << convertLPWSTRToString(_audioStreamIndex) << ". Using the default stream '" << defaultDeviceName << "' now.";
                        Log::w(ss.str());
                    }else{
                        Log::e("WASAPI: Failed to get the default device. Recording was stopped.");
                        //stop recording
                        pDeviceCollection->Release();
                        pEnumerator->Release();
                        CoUninitialize();
                        return;
                    }
                }

                IAudioClient* pAudioClient = nullptr;
                hr = device->Activate(IID_IAudioClient, CLSCTX_ALL,NULL, (void**)& pAudioClient);
                if(!FAILED(hr)){
                    //get the wave format
                    WAVEFORMATEX* waveFormat = nullptr;
                    hr = pAudioClient->GetMixFormat(&waveFormat);
                    if(!FAILED(hr)){
                        //set the sample rate
                        waveFormat->nSamplesPerSec = _sampleRate;
                        WORD channelCount = waveFormat->nChannels;
                        hr = pAudioClient->Initialize(
                                AUDCLNT_SHAREMODE_SHARED,
                                AUDCLNT_STREAMFLAGS_LOOPBACK,
                                BUFFER_SIZE,
                                0,
                                waveFormat,
                                NULL);
                        if(!FAILED(hr)){
                            //get the maximum number of frames in the buffer
                            UINT32 bufferFrameCount;
                            hr = pAudioClient->GetBufferSize(&bufferFrameCount);
                            if(!FAILED(hr)){
                                //get the capture client
                                IAudioCaptureClient* pCaptureClient = nullptr;
                                hr = pAudioClient->GetService(IID_IAudioCaptureClient,(void**)& pCaptureClient);
                                if(!FAILED(hr)){
                                    std::stringstream bfc;
                                    bfc << "The BufferFrameCount was automatically set to: " << bufferFrameCount;
                                    Log::d(bfc.str());
                                    // Calculate the actual duration of the allocated buffer.
                                    REFERENCE_TIME hnsActualDuration = waveFormat->nSamplesPerSec / bufferFrameCount;
                                    //start the audio capture
                                    hr = pAudioClient->Start();
                                    if(!FAILED(hr)){

                                        //do recording
                                        bool running = true;
                                        BYTE* pData;
                                        UINT32 packetLength = 0;
                                        DWORD flags;
                                        UINT32 numFramesAvailable;
                                        float buffer[BUFFER_SIZE * 2];
                                        unsigned int bufferedSamples = 0;
                                        //process the buffer
                                        while(running){
                                            //sleep for half the buffer duration.
                                            Sleep(hnsActualDuration / 4);
                                            hr = pCaptureClient->GetNextPacketSize(&packetLength);
                                            if(FAILED(hr)){
                                                Log::e("WASAPI: Could not get the next packet size.");
                                                break;
                                            }
                                            while (packetLength != 0){
                                                //get the next chunk of data
                                                hr = pCaptureClient->GetBuffer(
                                                        &pData,
                                                        &numFramesAvailable,
                                                        &flags, NULL, NULL);
                                                if(FAILED(hr)){
                                                    Log::w("WASAPI: Could not read a buffer.");
                                                    break;
                                                }
                                                //check for no silence
                                                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)){
                                                    //append the new samples to the end of the buffer
                                                    for(unsigned int i = 0; i < numFramesAvailable; i++){
                                                        buffer[i + bufferedSamples] = reinterpret_cast<float*>(pData)[i * channelCount];
                                                    }
                                                    bufferedSamples += numFramesAvailable;
                                                }
                                                //check if the buffer is full
                                                if(bufferedSamples >= BUFFER_SIZE){
                                                    //check if the other thread was fast enough with processing the previous buffer
                                                    if(sampleBuffer.empty()){
                                                        //copy data into another sample buffer
                                                        for (unsigned int i = 0; i < bufferedSamples; i++)
                                                        {
                                                            sampleBuffer.push_back(buffer[i]);
                                                        }
                                                        //notify the audio processor about a new set of samples
                                                        {
                                                            std::lock_guard<std::mutex> lock(std::mutex);
                                                            AudioProcessor::bufferReady = true;
                                                        }
                                                        conditionVariable.notify_one();
                                                    }else{
                                                        std::stringstream ss;
                                                        ss << "Too slow, skipping " << bufferedSamples << " samples.";
                                                        //skip the next couple of samples
                                                        Log::d(ss.str());
                                                    }
                                                    //reset the buffer
                                                    bufferedSamples = 0;
                                                }

                                                hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
                                                if(FAILED(hr)){
                                                    Log::e("WASAPI: Could not release a buffer.");
                                                    break;
                                                }
                                                hr = pCaptureClient->GetNextPacketSize(&packetLength);
                                                if(FAILED(hr)){
                                                    Log::e("WASAPI: Could not get the next packet size for processing.");
                                                    break;
                                                }
                                            }

                                            //check if this thread needs to be stopped
                                            {
                                                std::lock_guard<std::mutex> lock(interruptRecording);
                                                running = shouldRecord;
                                            }
                                        }

                                        //stop recording
                                        hr = pAudioClient->Stop();
                                        if(FAILED(hr)){
                                            Log::e("WASAPI: Audio client could not be stopped.");
                                        }
                                    }else{
                                        Log::e("WASAPI: Could not start the audio capture.");
                                    }
                                }else{
                                    Log::e("WASAPI: Could not get the capture client.");
                                }
                                pCaptureClient->Release();
                            }else{
                                Log::e("WASAPI: Could not get the buffer frame count.");
                            }
                        }else{
                            Log::e("WASAPI: Could not initialize the audio client.");
                        }
                    }else{
                        Log::e("WASAPI: Failed to get the wave format of a device.");
                    }
                }else{
                    Log::e("WASAPI: Could not activate an audio client");
                }
                pAudioClient->Release();
                device->Release();
            }else if(!FAILED(hr)){
                Log::w("WASAPI: No active audio devices were found.");
            }else{
                Log::e("WASAPI: Could not get the amount of active devices.");
            }
            pDeviceCollection->Release();
        }else{
            Log::e("WASAPI: Could not obtain collection of active devices.");
        }
        pEnumerator->Release();
    }else{
        Log::e("WASAPI: Could not create DeviceEnumerator.");
    }
    CoUninitialize();
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
            std::lock_guard<std::mutex> lock(interruptProcessing);
            running = AudioProcessor::shouldProcess;
        }
    }
    Log::d("Stopped sample processing thread.");
}

void AudioProcessor::stop() {
    //stop the recording thread
    {
        std::lock_guard<std::mutex> lock(interruptRecording);
        AudioProcessor::shouldRecord = false;
    }
    recorder.join();

    //stop the processing thread
    {
        std::lock_guard<std::mutex> lock(interruptProcessing);
        AudioProcessor::shouldProcess = false;
    }
    //notify the audio processor about a new set of samples although there are none to prevent a deadlock
    {
        std::lock_guard<std::mutex> lock(std::mutex);
        AudioProcessor::bufferReady = true;
    }
    AudioProcessor::conditionVariable.notify_one();
    //then wait for the thread to finish
    processor.join();
}

//endregion

std::vector<std::string> AudioProcessor::printAudioStreams(const std::vector<std::string> &blackList) {
    std::vector<std::string> streamNames;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    //get device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (!FAILED(hr)) {
        //get all devices
        IMMDeviceCollection* pDeviceCollection = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDeviceCollection);
        if(!FAILED(hr)){
            //get the device count
            UINT deviceCount = 0;
            hr = pDeviceCollection->GetCount(&deviceCount);
            if (!FAILED(hr) && deviceCount > 0) {
                //resize the stream names vector
                streamNames.resize(deviceCount);
                //go through all devices
                for (UINT i = 0; i < deviceCount; ++i) {

                    //get one device
                    IMMDevice* pDevice = nullptr;
                    hr = pDeviceCollection->Item(i, &pDevice);
                    if (!FAILED(hr)) {
                        //get the device id
                        LPWSTR pDeviceId = nullptr;
                        hr = pDevice->GetId(&pDeviceId);
                        if(!FAILED(hr)){
                            //get the device properties
                            IPropertyStore* pPropertyStore = nullptr;
                            hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
                            if(!FAILED(hr)){
                                //get the name of the device
                                PROPVARIANT propVariant;
                                PropVariantInit(&propVariant);
                                hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propVariant);
                                if (!FAILED(hr)) {
                                    //print the device name and add it to the list
                                    std::string deviceName = convertLPWSTRToString(propVariant.pwszVal);
                                    streamNames[i] = deviceName;
                                    std::cout << "[" << i << "]" << " Name: '" << deviceName << std::endl;
                                }else{
                                    Log::e("WASAPI: Could not get a device name.");
                                }
                                pPropertyStore->Release();
                                PropVariantClear(&propVariant);
                            }else{
                                Log::e("WASAPI: Could not get the properties of a device.");
                            }
                            CoTaskMemFree(pDeviceId);
                        }else{
                            Log::e("WASAPI: Could not get the id of a device.");
                        }
                        pDevice->Release();
                    }else{
                        Log::w("WASAPI: Could not get a device from the collection.");
                    }

                }
            }else if(!FAILED(hr)){
                Log::w("WASAPI: No active audio devices were found.");
            }else{
                Log::e("WASAPI: Could not get the amount of active devices.");
            }
            pDeviceCollection->Release();
        }else{
            Log::e("WASAPI: Could not obtain collection of active devices.");
        }
        pEnumerator->Release();
    }else{
        Log::e("WASAPI: Could not create DeviceEnumerator.");
    }

    CoUninitialize();
    return streamNames;
}

double AudioProcessor::setAudioStreamByName(std::string name) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    //get device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (!FAILED(hr)) {
        //get all devices
        IMMDeviceCollection* pDeviceCollection = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDeviceCollection);
        if(!FAILED(hr)){
            //get the device count
            UINT deviceCount = 0;
            hr = pDeviceCollection->GetCount(&deviceCount);
            if (!FAILED(hr) && deviceCount > 0) {
                //go through all devices
                for (UINT i = 0; i < deviceCount; ++i) {

                    //get one device
                    IMMDevice* pDevice = nullptr;
                    hr = pDeviceCollection->Item(i, &pDevice);
                    if (!FAILED(hr)) {
                        //get the device id
                        LPWSTR pDeviceId = nullptr;
                        hr = pDevice->GetId(&pDeviceId);
                        if(!FAILED(hr)){
                            //get the device properties
                            IPropertyStore* pPropertyStore = nullptr;
                            hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
                            if(!FAILED(hr)){
                                //get the name of the device
                                PROPVARIANT propertyName;
                                PropVariantInit(&propertyName);
                                hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyName);
                                if (!FAILED(hr)) {
                                    //print the device name and add it to the list
                                    std::string deviceName = convertLPWSTRToString(propertyName.pwszVal);
                                    if(name.compare(deviceName) == 0){
                                        //activate the audio client to get the default sample rate
                                        IAudioClient* pAudioClient = nullptr;
                                        hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL,nullptr, (void**)& pAudioClient);
                                        if(!FAILED(hr)){
                                            //get the wave format
                                            WAVEFORMATEX* waveFormat = nullptr;
                                            hr = pAudioClient->GetMixFormat(&waveFormat);
                                            if(!FAILED(hr)){
                                                //set the device id
                                                _audioStreamIndex = pDeviceId;
                                                //save the sample rate
                                                _sampleRate = waveFormat->nSamplesPerSec;
                                                //release everything before returning
                                                pAudioClient->Release();
                                                pPropertyStore->Release();
                                                PropVariantClear(&propertyName);
                                                CoTaskMemFree(pDeviceId);
                                                pDevice->Release();
                                                pDeviceCollection->Release();
                                                pEnumerator->Release();
                                                CoUninitialize();
                                                return convertDWORDtoDouble(_sampleRate);
                                            }else{
                                                Log::e("WASAPI: Failed to get the wave format of a device.");
                                            }
                                        }else{
                                            Log::e("WASAPI: Could not activate an audio client");
                                        }
                                        pAudioClient->Release();
                                    }
                                }else{
                                    Log::e("WASAPI: Could not get a device name.");
                                }
                                pPropertyStore->Release();
                                PropVariantClear(&propertyName);
                            }else{
                                Log::e("WASAPI: Could not get the properties of a device.");
                            }
                            CoTaskMemFree(pDeviceId);
                        }else{
                            Log::e("WASAPI: Could not get the id of a device.");
                        }
                        pDevice->Release();
                    }else{
                        Log::w("WASAPI: Could not get a device from the collection.");
                    }

                }
            }else if(!FAILED(hr)){
                Log::w("WASAPI: No active audio devices were found.");
            }else{
                Log::e("WASAPI: Could not get the amount of active devices.");
            }
            pDeviceCollection->Release();
        }else{
            Log::e("WASAPI: Could not obtain collection of active devices.");
        }
        pEnumerator->Release();
    }else{
        Log::e("WASAPI: Could not create DeviceEnumerator.");
    }

    CoUninitialize();


    std::stringstream ss;
    ss << "The audio stream with name '" << name << "' could not be found.";
    Log::e(ss.str());
    return 0;
}

bool AudioProcessor::setSampleRate(double sampleRate) {
    if(sampleRate > 60 && sampleRate < 400000){
        _sampleRate = sampleRate;
        return true;
    }else{
        std::stringstream ss;
        ss << "The sample rate of " << sampleRate << "Hz is out of bounds.";
        Log::e(ss.str());
    }
    return false;
}

std::string AudioProcessor::convertLPWSTRToString(LPWSTR lpwszStr) {
    if(lpwszStr == nullptr){
        return "nullptr";
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, lpwszStr, -1, nullptr, 0, nullptr, nullptr);
    if (len == 0) {
        Log::e("Could not convert LPWSTR to std::string. The LPWSTR was empty.");
        return "";
    }

    std::string str(len - 1, 0);
    if (WideCharToMultiByte(CP_UTF8, 0, lpwszStr, -1, &str[0], len, nullptr, nullptr) == 0) {
        Log::e("Could not convert LPWSTR to std::string.");
        return "";
    }

    return str;
}

double AudioProcessor::convertDWORDtoDouble(DWORD dwValue) {
    return static_cast<double>(dwValue);
}

DWORD AudioProcessor::convertDoubleToDWORD(double doubleValue) {
    //check if the double value is not out of bounds
    if (doubleValue < 0.0 || doubleValue > static_cast<double>(std::numeric_limits<DWORD>::max())) {
        Log::e("The double value is out of bounds. Cannot convert it to DWORD.");
        return 0;
    }

    return static_cast<DWORD>(doubleValue);
}

std::string AudioProcessor::getDeviceName(IMMDevice *pDevice) {
    //get the device properties
    IPropertyStore* pPropertyStore = nullptr;
    HRESULT hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    if(!FAILED(hr)){
        //get the name of the device
        PROPVARIANT propertyName;
        PropVariantInit(&propertyName);
        hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propertyName);
        if (!FAILED(hr)) {
            return convertLPWSTRToString(propertyName.pwszVal);
        }else{
            Log::e("WASAPI: Could not get a device name.");
        }
        pPropertyStore->Release();
        PropVariantClear(&propertyName);
    }else{
        Log::e("WASAPI: Could not get the properties of a device.");
    }
    return "undefined";
}

DWORD AudioProcessor::getDeviceSampleRate(LPWSTR deviceIndex) {
    DWORD sampleRate = 0;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    //get device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (!FAILED(hr)) {
        //get all devices
        IMMDeviceCollection* pDeviceCollection = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDeviceCollection);
        if(!FAILED(hr)){
            //get the device count
            UINT deviceCount = 0;
            hr = pDeviceCollection->GetCount(&deviceCount);
            if (!FAILED(hr) && deviceCount > 0) {
                //go through all devices
                for (UINT i = 0; i < deviceCount; ++i) {

                    //get one device
                    IMMDevice* pDevice = nullptr;
                    hr = pDeviceCollection->Item(i, &pDevice);
                    if (!FAILED(hr)) {
                        //get the device id
                        LPWSTR pDeviceId = nullptr;
                        hr = pDevice->GetId(&pDeviceId);
                        if(!FAILED(hr)){
                            //check if the device was found
                            if(wcscmp(pDeviceId, deviceIndex) == 0){
                                //activate the audio client to get the default sample rate
                                IAudioClient* pAudioClient = nullptr;
                                hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL,nullptr, (void**)& pAudioClient);
                                if(!FAILED(hr)){
                                    //get the wave format
                                    WAVEFORMATEX* waveFormat = nullptr;
                                    hr = pAudioClient->GetMixFormat(&waveFormat);
                                    if(!FAILED(hr)){
                                        //get the default sample rate for the device
                                        sampleRate = waveFormat->nSamplesPerSec;
                                    }else{
                                        Log::e("WASAPI: Failed to get the wave format of a device.");
                                    }
                                    pAudioClient->Release();
                                }else{
                                    Log::e("WASAPI: Could not activate an audio client");
                                }
                                CoTaskMemFree(pDeviceId);
                                pDevice->Release();
                                break;
                            }
                            CoTaskMemFree(pDeviceId);
                        }else{
                            Log::e("WASAPI: Could not get the id of a device.");
                        }
                        pDevice->Release();
                    }else{
                        Log::w("WASAPI: Could not get a device from the collection.");
                    }

                }
            }else if(!FAILED(hr)){
                Log::w("WASAPI: No active audio devices were found.");
            }else{
                Log::e("WASAPI: Could not get the amount of active devices.");
            }
            pDeviceCollection->Release();
        }else{
            Log::e("WASAPI: Could not obtain collection of active devices.");
        }
        pEnumerator->Release();
    }else{
        Log::e("WASAPI: Could not create DeviceEnumerator.");
    }

    CoUninitialize();
    return sampleRate;
}

LPWSTR AudioProcessor::getDefaultStreamIndex() {
    LPWSTR id = nullptr;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    //get device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (!FAILED(hr)) {
        //get the default stream
        IMMDevice* device = nullptr;
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        if(!FAILED(hr)){
            //get the device id
            LPWSTR pDeviceId = nullptr;
            hr = device->GetId(&pDeviceId);
            if(!FAILED(hr)){
                id = pDeviceId;
            }else{
                Log::w("WASAPI: Could not get a device from the collection.");
            }
            device->Release();
        }else{
            Log::e("WASAPI: Failed to get the default audio device.");
        }
        pEnumerator->Release();
    }else{
        Log::e("WASAPI: Could not create DeviceEnumerator.");
    }

    CoUninitialize();
    return id;
}

DWORD AudioProcessor::getDeviceSampleRate(IMMDevice *pDevice) {
    DWORD sampleRate = 0;

    //activate the audio client to get the default sample rate
    IAudioClient* pAudioClient = nullptr;
    HRESULT hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL,nullptr, (void**)& pAudioClient);
    if(!FAILED(hr)){
        //get the wave format
        WAVEFORMATEX* waveFormat = nullptr;
        hr = pAudioClient->GetMixFormat(&waveFormat);
        if(!FAILED(hr)){
            //get the default sample rate for the device
            sampleRate = waveFormat->nSamplesPerSec;
        }else{
            Log::e("WASAPI: Failed to get the wave format of a device.");
        }
        pAudioClient->Release();
    }else{
        Log::e("WASAPI: Could not activate an audio client");
    }

    return sampleRate;
}

