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
#include "portaudio.h"
#include "Effect.h"

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1470 //882 for 50fps, 1470 for 30fps

class AudioProcessor{
private:
    //singleton
    AudioProcessor();
    inline static AudioProcessor *instance = nullptr;
    //audio members
    PaStream* stream;
    int _audioStreamIndex;
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
    void printAudioStreams();
    bool setAudioStreamIndex(int index);
};


#endif //WLED_AUDIOREACT_AUDIOPROCESSOR_H
