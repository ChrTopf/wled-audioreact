//
// Created by chrtopf on 18.06.23.
//

#include "AverageVolumeEffect.h"
#include <cmath>

#define SPEED_MULTIPLIER 0.5

void AverageVolumeEffect::onData(const vector<float> &data) {
    //go through all the samples
    float cumulated = 0;
    for(int i = 0; i < data.size(); i++){
        cumulated += abs(data[i]);
    }
    //calculate the current currentVal
    float currentVal = cumulated / data.size();

    //reset the maximum value once the music has stopped playing
    if(_secondLastVal == 0 && _lastVal == 0 && currentVal == 0){
        _maxVal = 0;
        return;
    }

    //calculate the
    //currentVal = 20 * log10(currentVal);
    //currentVal = abs(1 / currentVal);

    //implement causal system with delay
    currentVal = 0.5 * (_lastVal - currentVal) + 0.25 * _lastVal + currentVal;

    //save last values
    _secondLastVal = _lastVal;
    _lastVal = currentVal;
    //adjust the maximum value very slowly to counter volume decreasing (if sb makes the music more quiet)
    _maxVal -= 0.0001 * currentVal;


    float cSpeed;
    int height;
    //check if no audio is playing right now
    if(currentVal < 0.01){
        height = 0;
        //color speed factor
        cSpeed = 0.01 * SPEED_MULTIPLIER;
    }else{
        //check if this is a new record in volume
        if(currentVal > _maxVal){
            _maxVal = currentVal;
        }
        //calculate the next height
        height = (int) (144 * (currentVal / _maxVal)); // LED_COUNT
        //color speed factor
        cSpeed = (currentVal / _maxVal) * SPEED_MULTIPLIER;
    }

    //calculate the next color
    if(_r > 0 && _b <= 0){
        _r -= cSpeed;
        _g += cSpeed;
    }
    if(_g > 0 && _r <= 0){
        _g -= cSpeed;
        _b += cSpeed;
    }
    if(_b > 0 && _g <= 0){
        _b -= cSpeed;
        _r += cSpeed;
    }
    //calculate the new height
    for(int i = 0; i < height; i++){
        red[i] = (char) round(_r);
        green[i] = (char) round(_g);
        blue[i] = (char) round(_b);
    }
    //fill black space
    for(int i = height; i < 144; i++){
        red[i] = 0;
        green[i] = 0;
        blue[i] = 0;
    }

    //now send the data
    _network->sendData(red, green, blue);
}
