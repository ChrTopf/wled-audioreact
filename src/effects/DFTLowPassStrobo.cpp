//
// Created by chrtopf on 17.08.23.
//

#include "DFTLowPassStrobo.h"
//DFT = discrete fourier transform
//the maximum audible frequency in Hz
#define FREQUENCY_SPECTRUM 20000
#define MINIMUM_100HZ 0.003
#define MAXIMUM_100HZ 0.0065
#define MINIMUM_500HZ 0.015
#define MAXIMUM_500HZ 0.0375
//the threshold to be reached for a peak to be detected
#define MAX_VALUE_MARGIN 0.6
//the rate of depletion/blur for the effect between 0.9 and 0.1
#define STROBO_DEPLETION_FACTOR 0.4

DFTLowPassStrobo::DFTLowPassStrobo() {
    lastBrightness = 0;
    _maxValueThreshold = 1;
}

void DFTLowPassStrobo::onData(const std::vector<float> &data) {
    unsigned long n = data.size();
    unsigned long outSize = (n/2+1);
    //create a plan for the dft
    auto *in = new double[n];
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * outSize);
    p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
    //parse the data into the input of the fft
    for(int i = 0; i < n; i++){
        in[i] = data[i];
    }

    fftw_execute(p); /* repeat as needed */

    //low pass filter
    _100Minimum = outSize * MINIMUM_100HZ;
    _100Maximum = outSize * MAXIMUM_100HZ;
    _500Minimum = outSize * MINIMUM_500HZ;
    _500Maximum = outSize * MAXIMUM_500HZ;
    //std::cout << _100Minimum << " " << _100Maximum << " " << _500Minimum << " " << _500Maximum << std::endl;
    double maximum = 0;
    unsigned long indexOfMaximum = 0;
    for(int i = 1; i < outSize; i++){
        //only get the real part (index 0)
        double currentValue = std::abs(out[i][0]);
        if(currentValue > maximum && currentValue > _maxValueThreshold){
            maximum = currentValue;
            indexOfMaximum = i;
        }
    }
    //check if the maximum is somewhere in the low frequencies and blur the strobo effect
    char8_t brightness;
    if((indexOfMaximum >= _100Minimum && indexOfMaximum <= _100Maximum) || (indexOfMaximum >= _500Minimum && indexOfMaximum <= _500Maximum)){
        brightness = 255;
        _maxValueThreshold = maximum * MAX_VALUE_MARGIN;
    }else{
        if(brightness > 0){
            brightness = STROBO_DEPLETION_FACTOR * lastBrightness;
        }
    }
    lastBrightness = brightness;

    //send data
    for(int i = 0; i < LED_AMOUNT; i++){
        _red[i] = brightness;
        _green[i] = brightness;
        _blue[i] = brightness;
    }
    _network->sendData(_red, _green, _blue);

    //tidy up
    fftw_destroy_plan(p);
    delete[] in;
    fftw_free(out);
}
