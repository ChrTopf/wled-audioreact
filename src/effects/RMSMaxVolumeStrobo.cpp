//
// Created by chrtopf on 16.10.23.
//

#include "RMSMaxVolumeStrobo.h"
#include <cmath>

//the threshold to be reached for a peak to be detected
#define DELTA_THRESHOLD 0.5
//the rate of depletion/blur for the effect between 0.9 and 0.1
#define STROBO_DEPLETION_FACTOR 0.3
//define the absolute brightness
#define BRIGHTNESS 1

void RMSMaxVolumeStrobo::onData(const std::vector<float> &data) {
    //calculate current rms value
    float currentRMS = 0;
    for(int i = 0; i < data.size(); i++){
        float val = data[i];
        currentRMS += val*val;
    }
    currentRMS = std::sqrt(currentRMS / ((float) data.size() / 2));

    //reset the maximum value and do not show any effect
    if(currentRMS == 0){
        _lastBrightness = 0;
        _biggestDelta = 0.0;
        return;
    }

    //adjust the current rms value
    currentRMS = std::log10(currentRMS)*20;
    currentRMS = std::abs(1/currentRMS);

    char8_t brightness;
    //check if no audio is playing right now
    if(currentRMS < 0.01){
        brightness = 0;
    }else{
        //calculate the change in volume
        float delta = currentRMS - _lastVal;
        //check if this is a new peak in volume
        if(delta >= (_biggestDelta * DELTA_THRESHOLD)){
            _biggestDelta = delta;
            brightness = 255;
        }else{
            //fade out
            if(brightness > 0){
                brightness = STROBO_DEPLETION_FACTOR * _lastBrightness;
            }
        }
        //deplete the biggest delta very slowly to counter volume decreasing (if sb makes the music more quiet)
        _biggestDelta -= 0.0001 * delta;
    }
    _lastBrightness = brightness;

    //limit the brightness
    brightness = BRIGHTNESS * brightness;

    //calculate the next brightness
    for(int i = 0; i < LED_AMOUNT; i++){
        _red[i] = brightness;
        _green[i] = brightness;
        _blue[i] = brightness;
    }
    //now send the data
    _network->sendData(_red, _green, _blue);

    //save last values
    _lastVal = currentRMS;
}
