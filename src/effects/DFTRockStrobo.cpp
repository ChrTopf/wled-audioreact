//
// Created by chrtopf on 17.08.23.
//

#include "DFTRockStrobo.h"
//DFT = discrete fourier transform
//frequency bounds for the fft
#define MINIMUM_100HZ 60
#define MAXIMUM_100HZ 130
#define MINIMUM_500HZ 301
#define MAXIMUM_500HZ 750
//the threshold to be reached for a peak to be detected
#define DELTA_THRESHOLD 0.80
//the rate of depletion/blur for the effect between 0.9 and 0.1
#define STROBO_DEPLETION_FACTOR 0.3
//define the absolute brightness
#define BRIGHTNESS 1

DFTRockStrobo::DFTRockStrobo() {
    lastBrightness = 0;
    _maxValue100 = 20;
    _maxValue500 = 20;
}

void DFTRockStrobo::onData(const std::vector<float> &data) {
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

    //check if no music is playing
    if (out[0][0] == 0) {
        //reset the parameters
        lastBrightness = 0;
        _maxValue100 = 20;
        _maxValue500 = 20;
    }

    unsigned long frequencyBandwidth = (unsigned long) SAMPLE_RATE / n;
    //low pass filter
    _100Minimum = MINIMUM_100HZ / frequencyBandwidth;
    _100Maximum = MAXIMUM_100HZ / frequencyBandwidth;
    _500Minimum = MINIMUM_500HZ / frequencyBandwidth;
    _500Maximum = MAXIMUM_500HZ / frequencyBandwidth;
    //std::cout << _100Minimum << " " << _100Maximum << " " << _500Minimum << " " << _500Maximum << std::endl;
    double sum100 = 0;
    unsigned int size100 = 0;
    //check the lower base spectrum for maxima
    for (unsigned long i = _100Minimum; i <= _100Maximum; i++) {
        //only get the real part (index 0)
        sum100 += sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        size100++;
    }
    double sum500 = 0;
    unsigned int size500 = 0;
    //check the upper base spectrum for maxima
    for (unsigned long i = _500Minimum; i <= _500Maximum; i++) {
        //only get the real part (index 0)
        sum500 += sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        size500++;
    }
    //calculate the average value
    double average100 = sum100 / size100;
    double average500 = sum500 / size500;

    //std::cout << "Maximum100: " << average100 << " Maximum500: " << average500 << std::endl;

    //check if a maximum was discovered in the low frequencies
    char8_t brightness;
    if (average100 > (_maxValue100 * DELTA_THRESHOLD)) {
        //set maximum
        brightness = 255;
        _maxValue100 = average100;
    } else if (average500 > (_maxValue500 * DELTA_THRESHOLD)){
        brightness = 255;
        _maxValue500 = average500;
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
        _green[i] = brightness;
        _blue[i] = brightness;
    }
    _network->sendData(_red, _green, _blue);

    //tidy up
    fftw_destroy_plan(p);
    delete[] in;
    fftw_free(out);
}
