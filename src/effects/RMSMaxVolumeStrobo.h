//
// Created by chrtopf on 16.10.23.
//

#ifndef WLED_AUDIOREACT_RMSMAXVOLUMESTROBO_H
#define WLED_AUDIOREACT_RMSMAXVOLUMESTROBO_H


#include "../Effect.h"

class RMSMaxVolumeStrobo : public Effect{
private:
    float _r = 255.0, _g = 0.0, _b = 0.0;
    float _biggestDelta = 0.0;
    float _lastVal = 0.0;
    char8_t _lastBrightness = 0;
public:
    RMSMaxVolumeStrobo(): Effect(){}
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_RMSMAXVOLUMESTROBO_H
