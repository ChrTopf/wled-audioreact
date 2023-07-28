//
// Created by chrtopf on 07.06.23.
//

#ifndef WLED_AUDIOREACT_SIGNALCONTROLLER_H
#define WLED_AUDIOREACT_SIGNALCONTROLLER_H
#include <string>
#include "NetworkHandler.h"
#if __has_include("win/AudioProcessor.h")
#include "win/AudioProcessor.h"
#else
#include "linux/AudioProcessor.h"
#endif
#include "WLEDSocket.h"
#include "Config.h"

class SignalController {
private:
    NetworkHandler _network;
    AudioProcessor *_processor;
    Config _config;
    std::vector<std::string> _blacklist;
    void setEffect(Effect *effect);
    inline static void printEffect(const int index, const std::string &text){
        std::cout << "[" << index << "]" << " " << text << std::endl;
    }
public:
    SignalController(const std::vector<std::string> &addressees, int ledAmount, Config &config);
    ~SignalController();
    void chooseAudioStream();
    void chooseEffect();
    bool startStreaming();
    void stopStreaming();
    void userSetNewAudioIndex();
    void userSetNewEffectIndex();
};


#endif //WLED_AUDIOREACT_SIGNALCONTROLLER_H
