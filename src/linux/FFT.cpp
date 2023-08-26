//
// Created by ChrTopf on 26.08.2023.
//

#include "FFT.h"

void FFT::realToAbsoluteDFT(const std::vector<float> &input, std::vector<double> &output) {
    unsigned long n = data.size();
    //std::cout << n << std::endl;
    unsigned long outSize = (n / 2 + 1);
    //create a plan for the dft
    auto *in = new double[n];
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * outSize);
    p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
    //parse the data into the input of the fft
    for (int i = 0; i < n; i++) {
        in[i] = data[i];
    }

    for (int i = 0; i < n; i++) {
        fftw_execute(p); /* repeat as needed */
    }

    sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);

    //tidy up
    fftw_destroy_plan(p);
    delete[] in;
    fftw_free(out);
}
