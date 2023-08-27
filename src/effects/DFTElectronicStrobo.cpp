//
// Created by chrtopf on 22.08.23.
//

#include "DFTElectronicStrobo.h"
//DFT = discrete fourier transform
//frequency bounds for the fft
#define MINIMUM_100HZ 60
#define MAXIMUM_100HZ 130
#define MINIMUM_500HZ 301
#define MAXIMUM_500HZ 750
//the threshold to be reached for a peak to be detected
#define MAX_VALUE_THRESHOLD 0.80
//the rate of depletion/blur for the effect between 0.9 and 0.1
#define STROBO_DEPLETION_FACTOR 0.3
//define the absolute brightness
#define BRIGHTNESS 0.4

DFTElectronicStrobo::DFTElectronicStrobo() {
    lastBrightness = 0;
    _maxValue = 20;
}

void DFTElectronicStrobo::onData(const std::vector<float> &data) {
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
        _maxValue = 20;
    }

    unsigned long frequencyBandwidth = (unsigned long) SAMPLE_RATE / n;
    //std::cout << _100Minimum << " " << _100Maximum << " " << _500Minimum << " " << _500Maximum << std::endl;
    //filter low frequencies
    double sum = 0;
    unsigned int size = 0;
    //check the lower base spectrum for maxima
    for (unsigned long i = 1; i <= 5; i++) {
        //only get the real part (index 0)
        sum += sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        size++;
    }
    //calculate the average value
    double average = sum / size;

    //std::cout << "Maximum100: " << average << " Maximum500: " << average500 << std::endl;

    //check if a maximum was discovered in the low frequencies
    char8_t brightness;
    if (average > (_maxValue * MAX_VALUE_THRESHOLD)) {
        //set maximum
        brightness = 255;
        _maxValue = average;
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