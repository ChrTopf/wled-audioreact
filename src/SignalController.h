//
// Created by chrtopf on 07.06.23.
//

#ifndef WLED_AUDIOREACT_SIGNALCONTROLLER_H
#define WLED_AUDIOREACT_SIGNALCONTROLLER_H
#include <string>
#include "NetworkHandler.h"
#include "AudioProcessor.h"
#include "WLEDSocket.h"
#include "Config.h"


class SignalController {
private:
    NetworkHandler _network;
    AudioProcessor *_processor;
    Config _config;
    vector<string> _blacklist;
    void setEffect(Effect *effect);
    inline static void printEffect(const int index, const string &text){
        cout << "[" << index << "]" << " " << text << endl;
    }
public:
    SignalController(const std::vector<std::string> &addressees, const Config &config);
    ~SignalController();
    void chooseAudioStream();
    void chooseEffect();
    bool startStreaming();
    void stopStreaming();
    void userSetNewAudioIndex();
    void userSetNewEffectIndex();
};


#endif //WLED_AUDIOREACT_SIGNALCONTROLLER_H
