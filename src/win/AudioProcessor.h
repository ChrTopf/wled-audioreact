//
// Created by chrtopf on 07.06.23.
//

#ifndef WLED_AUDIOREACT_AUDIOPROCESSOR_H
#define WLED_AUDIOREACT_AUDIOPROCESSOR_H
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <cstring>
#include "../Effect.h"
#include "../Log.h"

#include <Windows.h>
#include <Mmdeviceapi.h>
#include <setupapi.h>
#include <initguid.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <Audiopolicy.h>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000
//the amount of 100ns to pass for one frame to be captured
#define BUFFER_DURATION 333334 //333334 for 30 fps (1/(100*10^-9*30)=333334)

class AudioProcessor{
private:
    //singleton
    AudioProcessor();
    inline static AudioProcessor *instance = nullptr;
    //audio members
    LPWSTR _audioStreamIndex;
    DWORD _sampleRate;
    //threading
    std::thread processor;
    std::mutex interruptProcessing;
    static inline bool shouldProcess = false; //needs to be static to work with two threads
    std::thread recorder;
    std::mutex interruptRecording;
    static inline bool shouldRecord = false; //needs to be static to work with two threads
    //effect
    std::mutex effectMutex;
    inline static Effect *_effect = nullptr; //needs to be static to work with two threads
    //internal stuff
    std::string convertLPWSTRToString(LPWSTR lpwszStr);
    double convertDWORDtoDouble(DWORD dwValue);
    DWORD convertDoubleToDWORD(double doubleValue);
    std::string getDeviceName(IMMDevice* pDevice);
    DWORD getDeviceSampleRate(IMMDevice* pDevice);
    DWORD getDeviceSampleRate(LPWSTR deviceIndex);
    LPWSTR getDefaultStreamIndex();
protected:
    //monitor concept for multithreading
    std::mutex audioBufferMutex;
    bool processorReady = false;
public:
    //singleton
    AudioProcessor(AudioProcessor const&) = delete;
    void operator=(AudioProcessor const&) = delete;
    static AudioProcessor *getInstance();
    static void destroy();
    //monitor concept for multithreading
    inline static std::vector<float> sampleBuffer = std::vector<float>();
    inline static std::condition_variable conditionVariable;
    inline static bool bufferReady = false;
    //setting effects
    void setEffect(Effect *effect);
    bool hasEffect();
    //core methods
    bool start();
    void onRun();
    void onProcessSamples();
    void stop();
    //audio stream selection
    std::vector<std::string> printAudioStreams(const std::vector<std::string> &blackList);
    /**
     * Set the current audio stream as source using its name.
     * @param name The name of the stream to be processed.
     * @return returns the default sample rate of that stream or 0 if it could not be configured.
     */
    double setAudioStreamByName(std::string name);
    /**
     * Set the sample rate for the current audio stream to be recorded at. It needs to lie between 60Hz and 400kHz.
     * @param sampleRate The sample rate to be chosen.
     * @return returns true if the sample rate could be set and false otherwise.
     */
    bool setSampleRate(double sampleRate);
};


#endif //WLED_AUDIOREACT_AUDIOPROCESSOR_H
