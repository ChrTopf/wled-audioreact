//
// Created by chrtopf on 07.06.23.
//

#ifndef WLED_AUDIOREACT_AUDIOPROCESSOR_H
#define WLED_AUDIOREACT_AUDIOPROCESSOR_H
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#if __has_include("portaudio.h")
#include "portaudio.h"
#else
#include "C:/Users/ChrTopf/Documents/repos/portaudio/include/portaudio.h"
#endif
#include <condition_variable>
#include "Effect.h"

#define BUFFER_SIZE 1470 //882 for 50fps, 1470 for 30fps

class AudioProcessor{
private:
    //singleton
    AudioProcessor();
    inline static AudioProcessor *instance = nullptr;
    //audio members
    PaStream* stream;
    int _audioStreamIndex;
    double _sampleRate;
    //threading
    std::thread processor;
    std::mutex interruptMutex;
    static inline bool shouldRun = false; //needs to be static to work with two threads
    //effect
    std::mutex effectMutex;
    inline static Effect *_effect = nullptr; //needs to be static to work with two threads
    //internal stuff
    void printPaError(const std::string &text, const PaError &error);
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
    void onProcessSamples();
    void stop();
    //audio stream selection
    vector<string> printAudioStreams(const vector<string> &blackList);
    /**
     * Set the current audio stream as source using its name.
     * @param name The name of the stream to be processed.
     * @return returns the default sample rate of that stream or 0 if it could not be configured.
     */
    double setAudioStreamByName(string name);
    /**
     * Set the sample rate for the current audio stream to be recorded at. It needs to lie between 60Hz and 400kHz.
     * @param sampleRate The sample rate to be chosen.
     * @return returns true if the sample rate could be set and false otherwise.
     */
    bool setSampleRate(double sampleRate);
};


#endif //WLED_AUDIOREACT_AUDIOPROCESSOR_H
