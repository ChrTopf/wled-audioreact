//
// Created by chrtopf on 17.08.23.
//

#ifndef WLED_AUDIOREACT_DFTROCKSTROBO_H
#define WLED_AUDIOREACT_DFTROCKSTROBO_H
#include "../Effect.h"
#include "fftw3.h"
#include <cmath>

class DFTRockStrobo : public Effect{
private:
    fftw_complex *out;
    fftw_plan p;
    double _maxValue100;
    double _maxValue500;
    unsigned long _100Minimum;
    unsigned long _100Maximum;
    unsigned long _500Minimum;
    unsigned long _500Maximum;
    char8_t lastBrightness;
public:
    DFTRockStrobo();
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_DFTROCKSTROBO_H
