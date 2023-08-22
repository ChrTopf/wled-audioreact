//
// Created by chrtopf on 22.08.23.
//

#ifndef WLED_AUDIOREACT_DFTELECTRONICSTROBO_H
#define WLED_AUDIOREACT_DFTELECTRONICSTROBO_H
#include "../Effect.h"
#include "fftw3.h"
#include "math.h"

class DFTElectronicStrobo : public Effect{
private:
    fftw_complex *out;
    fftw_plan p;
    double _maxValue;
    char8_t lastBrightness;
public:
    DFTElectronicStrobo();
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_DFTELECTRONICSTROBO_H
