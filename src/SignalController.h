//
// Created by chrtopf on 07.06.23.
//

#ifndef WLED_AUDIOREACT_SIGNALCONTROLLER_H
#define WLED_AUDIOREACT_SIGNALCONTROLLER_H
#include <string>
#include "NetworkHandler.h"
#include "AudioProcessor.h"
#include "WLEDSocket.h"


class SignalController {
private:
    NetworkHandler _network;
    AudioProcessor *_processor;
public:
    SignalController(const std::vector<std::string> &addressees);
    ~SignalController();
    void setEffect(Effect *effect);
    bool startStreaming();
    void stopStreaming();
};


#endif //WLED_AUDIOREACT_SIGNALCONTROLLER_H
