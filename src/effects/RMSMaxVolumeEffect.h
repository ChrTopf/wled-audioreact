//
// Created by chrtopf on 08.06.23.
//

#ifndef WLED_AUDIOREACT_RMSMAXVOLUMEEFFECT_H
#define WLED_AUDIOREACT_RMSMAXVOLUMEEFFECT_H
#include "../Effect.h"

class RMSMaxVolumeEffect : public Effect{
private:
    float _r = 255.0, _g = 0.0, _b = 0.0;
    float _maxVal = 0.0;
    float _lastVal = 0.0;
    float _secondLastVal = 0.0;
    float _trueRMSPeak = 0.0;
public:
    RMSMaxVolumeEffect(): Effect(){}
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_RMSMAXVOLUMEEFFECT_H
