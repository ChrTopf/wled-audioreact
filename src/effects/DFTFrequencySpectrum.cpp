//
// Created by chrtopf on 21.08.23.
//

#include "DFTFrequencySpectrum.h"

void DFTFrequencySpectrum::onData(const std::vector<float> &data) {
    unsigned long n = data.size();
    //std::cout << n << std::endl;

    fft.realToAbsoluteDFT(data, out);
    unsigned long outSize = out.size();

    //determine how the frequency bandwidth for each led
    unsigned int bandwidthPerLED = outSize / LED_AMOUNT;

    int h = 0;
    //send data
    for(int i = 0; i < LED_AMOUNT; i++){
        //cumulate every value for each LED
        double sum = 0;
        for(int k = 0; k < bandwidthPerLED; k++){
            sum += out[i];
            h++;
        }
        //calculate the temperature
        double temperature = (sum / bandwidthPerLED) * 2;
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
