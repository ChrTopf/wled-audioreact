//
// Created by chrtopf on 08.06.23.
//

#ifndef WLED_AUDIOREACT_EFFECT_H
#define WLED_AUDIOREACT_EFFECT_H
#include <vector>
#include "EffectParameters.h"
#include "NetworkHandler.h"

class Effect {
protected:
    char8_t *_red, *_green, *_blue;
    const int LED_AMOUNT;
    const double SAMPLE_RATE;
    NetworkHandler *_network = nullptr;
public:
    inline Effect(): LED_AMOUNT(EffectParameters::LED_AMOUNT), SAMPLE_RATE(EffectParameters::SAMPLE_RATE){
        _red = new char8_t[LED_AMOUNT];
        _green = new char8_t[LED_AMOUNT];
        _blue = new char8_t[LED_AMOUNT];
    }
    inline virtual ~Effect(){
        delete[] _red;
        delete[] _green;
        delete[] _blue;
    }
    inline void setNetworkHandler(NetworkHandler *network){_network = network;}
    virtual void onData(const std::vector<float> &data) = 0;
};


#endif //WLED_AUDIOREACT_EFFECT_H
