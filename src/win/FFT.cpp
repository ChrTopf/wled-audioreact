//
// Created by ChrTopf on 26.08.2023.
//
#include "FFT.h"

void FFT::realToAbsoluteDFT(const std::vector<float> &input, std::vector<double> &output) {
    unsigned long N = input.size();
    //clear the output array
    output.clear();
    //parse the input array
    Eigen::VectorXf in(N);
    for (int i = 0; i < N; ++i) {
        in(i) = input[i];
    }
    //create the output array
    Eigen::VectorXcf spectrum(N / 2 + 1);

    //execute dft only for one half
    Eigen::FFT<float> fft;
    fft.fwd(spectrum, in.head(N / 2 + 1));

    //parse the output array
    for (int i = 0; i < N / 2 + 1; ++i) {
        //add the absolute value to the output array
        output.push_back(sqrt(spectrum(i).real() * spectrum(i).real() + spectrum(i).imag() * spectrum(i).imag()));
    }
}
