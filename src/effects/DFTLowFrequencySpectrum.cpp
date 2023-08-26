//
// Created by chrtopf on 21.08.23.
//

#include "DFTLowFrequencySpectrum.h"

void DFTLowFrequencySpectrum::onData(const std::vector<float> &data) {
    unsigned long n = data.size();
    //std::cout << n << std::endl;

    FFT fft;
    fft.realToAbsoluteDFT(data, out);

    //send data
    for(int i = 0; i < LED_AMOUNT; i++){
        //calculate the temperature
        double temperature = out[i] * 2;
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
}
