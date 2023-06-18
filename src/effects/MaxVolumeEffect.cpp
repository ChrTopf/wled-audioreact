//
// Created by chrtopf on 18.06.23.
//

#include "MaxVolumeEffect.h"
#include <cmath>

#define SPEED_MULTIPLIER 0.5

void MaxVolumeEffect::onData(const vector<float> &data) {
    //go through all the samples
    float currentMax = 0;
    for(int i = 0; i < data.size(); i++){
        float val = abs(data[i]);
        if(val > currentMax){
            currentMax = val;
        }
    }

    //reset the maximum value once the music has stopped playing
    if(_secondLastVal == 0 && _lastVal == 0 && currentMax == 0){
        _maxVal = 0;
        return;
    }

    //calculate the
    //currentMax = 20 * log10(currentMax);
    //currentMax = abs(1 / currentMax);

    //implement causal system with delay
    currentMax = 0.25 * _secondLastVal + 0.5 * _lastVal + currentMax;
    //currentMax = 0.5 * (_lastVal - currentMax) + 0.25 * _lastVal + currentMax;

    //save last values
    _secondLastVal = _lastVal;
    _lastVal = currentMax;
    //adjust the maximum value very slowly to counter volume decreasing (if sb makes the music more quiet)
    _maxVal -= 0.0001 * currentMax;


    float cSpeed;
    int height;
    //check if no audio is playing right now
    if(currentMax < 0.01){
        height = 0;
        //color speed factor
        cSpeed = 0.01 * SPEED_MULTIPLIER;
    }else{
        //check if this is a new record in volume
        if(currentMax > _maxVal){
            _maxVal = currentMax;
        }
        //calculate the next height
        height = (int) (144 * (currentMax / _maxVal)); // LED_COUNT
        //color speed factor
        cSpeed = (currentMax / _maxVal) * SPEED_MULTIPLIER;
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