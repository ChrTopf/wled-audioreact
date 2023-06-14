//
// Created by chrtopf on 08.06.23.
//

#ifndef WLED_AUDIOREACT_EFFECT_H
#define WLED_AUDIOREACT_EFFECT_H
#include <vector>
#include "NetworkHandler.h"

class Effect {
protected:
    char red[144], green[144], blue[144];
    NetworkHandler *_network = nullptr;
public:
    inline void setNetworkHandler(NetworkHandler *network){_network = network;}
    virtual void onData(const std::vector<float> &data) = 0;
};


#endif //WLED_AUDIOREACT_EFFECT_H
