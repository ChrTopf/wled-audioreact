//
// Created by chrtopf on 08.06.23.
//

#ifndef WLED_AUDIOREACT_MAXVOLUMEEFFECT_H
#define WLED_AUDIOREACT_MAXVOLUMEEFFECT_H
#include "Effect.h"

class MaxVolumeEffect : public Effect{
private:
    int _r = 255,_g = 0,_b = 0;
    float _maxVal = 0.0;
public:
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_MAXVOLUMEEFFECT_H
