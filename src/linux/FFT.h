//
// Created by ChrTopf on 26.08.2023.
//

#ifndef WLED_AUDIOREACT_FFT_H
#define WLED_AUDIOREACT_FFT_H
#include <vector>
#include "fftw3.h"
#include <cmath>

class FFT {
private:
    fftw_complex *out;
    fftw_plan p;
public:
    void realToAbsoluteDFT(const std::vector<float> &input, std::vector<double> &output);
};


#endif //WLED_AUDIOREACT_FFT_H
