//
// Created by chrtopf on 21.08.23.
//

#include "DFTLowFrequencySpectrum.h"

void DFTLowFrequencySpectrum::onData(const std::vector<float> &data) {
    unsigned long n = data.size();
    //std::cout << n << std::endl;
    unsigned long outSize = (n / 2 + 1);
    //create a plan for the dft
    auto *in = new double[n];
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * outSize);
    p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
    //parse the data into the input of the fft
    for (int i = 0; i < n; i++) {
        in[i] = data[i] * 8;
    }

    for (int i = 0; i < n; i++) {
        fftw_execute(p); /* repeat as needed */
    }

    //send data
    for(int i = 0; i < LED_AMOUNT; i++){
        //calculate the temperature
        double temperature = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) * 2;
        //prevent index out of bounds
        if(temperature > 432){
            temperature = 432;
        }
        //calculate the color based on that
        unsigned int color = IRON_COLORS[(int) temperature];
        _red[i] = (color >> 16) & 0xff;
        _green[i] = (color >> 8) & 0xff;
        _blue[i] = color & 0xff;
    }

    _network->sendData(_red, _green, _blue);

    //tidy up
    fftw_destroy_plan(p);
    delete[] in;
    fftw_free(out);
}
