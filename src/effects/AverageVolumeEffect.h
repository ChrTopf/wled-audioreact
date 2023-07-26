//
// Created by chrtopf on 18.06.23.
//

#ifndef WLED_AUDIOREACT_AVERAGEVOLUMEEFFECT_H
#define WLED_AUDIOREACT_AVERAGEVOLUMEEFFECT_H
#include "../Effect.h"

class AverageVolumeEffect : public Effect{
private:
    float _r = 255.0, _g = 0.0, _b = 0.0;
    float _maxVal = 0.0;
    float _lastVal = 0.0;
    float _secondLastVal = 0.0;
public:
    AverageVolumeEffect(): Effect(){}
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_AVERAGEVOLUMEEFFECT_H
