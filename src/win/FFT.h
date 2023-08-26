//
// Created by ChrTopf on 26.08.2023.
//

#ifndef WLED_AUDIOREACT_FFT_H
#define WLED_AUDIOREACT_FFT_H
#include <vector>
#include <eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <cmath>

class FFT {
public:
    void realToAbsoluteDFT(const std::vector<float> &input, std::vector<double> &output);
};


#endif //WLED_AUDIOREACT_FFT_H
