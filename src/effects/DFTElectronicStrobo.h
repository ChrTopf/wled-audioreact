//
// Created by chrtopf on 22.08.23.
//

#ifndef WLED_AUDIOREACT_DFTELECTRONICSTROBO_H
#define WLED_AUDIOREACT_DFTELECTRONICSTROBO_H
#include "../Effect.h"
#include "FFT.h"
#include <cmath>

class DFTElectronicStrobo : public Effect{
private:
    FFT fft;
    std::vector<double> out;
    double _maxValue;
    char8_t lastBrightness;
public:
    DFTElectronicStrobo();
    void onData(const std::vector<float> &data) override;
};


#endif //WLED_AUDIOREACT_DFTELECTRONICSTROBO_H
