//
// Created by chrtopf on 17.08.23.
//

#include "DFTLowPassStrobo.h"
//DFT = discrete fourier transform
//frequency bounds for the fft
#define MINIMUM_100HZ 60
#define MAXIMUM_100HZ 130
#define MINIMUM_500HZ 301
#define MAXIMUM_500HZ 750
//the threshold to be reached for a peak to be detected
#define MAX_VALUE_MARGIN 0.6
//the rate of depletion/blur for the effect between 0.9 and 0.1
#define STROBO_DEPLETION_FACTOR 0.4
//define the absolute brightness
#define BRIGHTNESS 0.4

DFTLowPassStrobo::DFTLowPassStrobo() {
    lastBrightness = 0;
    _maxValueThreshold = 1;
}

void DFTLowPassStrobo::onData(const std::vector<float> &data) {
    unsigned long n = data.size();
    //std::cout << n << std::endl;
    unsigned long outSize = (n/2+1);
    //create a plan for the dft
    auto *in = new double[n];
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * outSize);
    p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
    //parse the data into the input of the fft
    for(int i = 0; i < n; i++){
        in[i] = data[i];
    }

    for(int i = 0; i < n; i++){
        fftw_execute(p); /* repeat as needed */
    }

    //check if no music is playing
    if(out[0][0] == 0){
        //reset the parameters
        lastBrightness = 0;
        _maxValueThreshold = 1;
    }

    unsigned long frequencyBandwidth = (unsigned long) SAMPLE_RATE / n;
    //low pass filter
    _100Minimum = MINIMUM_100HZ / frequencyBandwidth;
    _100Maximum = MAXIMUM_100HZ / frequencyBandwidth;
    _500Minimum = MINIMUM_500HZ / frequencyBandwidth;
    _500Maximum = MAXIMUM_500HZ / frequencyBandwidth;
    //std::cout << _100Minimum << " " << _100Maximum << " " << _500Minimum << " " << _500Maximum << std::endl;
    double maximum100 = 0;
    //check the lower base spectrum for maxima
    for(unsigned long i = _100Minimum; i <= _100Maximum; i++){
        //only get the real part (index 0)
        double currentValue = std::abs(out[i][0]);
        if(currentValue > maximum100 && currentValue > _maxValueThreshold){
            maximum100 = currentValue;
        }
    }
    double maximum500 = 0;
    //check the upper base spectrum for maxima
    for(unsigned long i = _500Minimum; i <= _500Maximum; i++){
        //only get the real part (index 0)
        double currentValue = std::abs(out[i][0]);
        if(currentValue > maximum500 && currentValue >= _maxValueThreshold){
            maximum500 = currentValue;
        }
    }
    /*
    if(maximum100 > 0 || maximum500 > 0){
        std::cout << "Maximum100: " << maximum100 << " Maximum500: " << maximum500 << std::endl;
    }
     */

    //check if a maximum was discovered in the low frequencies
    char8_t brightness;
    if(maximum100 > 0 || maximum500 > 0){
        //set maximum
        brightness = 255;
        if(maximum100 < maximum500){
            _maxValueThreshold = maximum500 * MAX_VALUE_MARGIN;
        }else{
            _maxValueThreshold = maximum100 * MAX_VALUE_MARGIN;
        }
    }else{
        //fade out
        if(brightness > 0){
            brightness = STROBO_DEPLETION_FACTOR * lastBrightness;
        }
    }
    lastBrightness = brightness;

    //limit the brightness
    brightness = BRIGHTNESS * brightness;

    //send data
    for(int i = 0; i < LED_AMOUNT; i++){
        _red[i] = brightness;
        _green[i] = 0;
        _blue[i] = 0;
    }
    _network->sendData(_red, _green, _blue);

    //tidy up
    fftw_destroy_plan(p);
    delete[] in;
    fftw_free(out);
}
