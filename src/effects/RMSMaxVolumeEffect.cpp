//
// Created by chrtopf on 08.06.23.
//
#include "RMSMaxVolumeEffect.h"
#include <cmath>

#define SPEED_MULTIPLIER 0.5

void RMSMaxVolumeEffect::onData(const std::vector<float> &data) {
    //go through all the samples
    float currentMax = 0;
    for(int i = 0; i < data.size(); i++){
        float val = abs(data[i]);
        if(val > currentMax){
            currentMax = val;
        }
    }
    //cout<<currentMax<<" sampleAbsMax | ";
    float currentRMS = 0;
    for(int i = 0; i < data.size(); i++){
        float val = data[i];
        currentRMS += val*val;
    }
    currentRMS = sqrt(currentRMS / ((float) data.size() / 2));
    //print the current maximum and rms value
    //cout<<currentRMS<<" sampleRMS | ";

    if(currentRMS == 0){
        _maxVal = 0;
        return;
    }
    currentRMS = log10(currentRMS)*20;
    //cout<<currentRMS<<" dBm"<<endl;
    currentRMS = abs(1/currentRMS);

    currentMax = log10(currentMax)*20;
    //cout<<currentMax<<" dBm_max"<<endl;
    currentMax = abs(1/currentMax);

    //implement causal system with delay
    //currentRMS = 0.25 * _secondLastVal + 0.5 * _lastVal + currentRMS;
    //currentRMS = 0.5 * (_lastVal - currentRMS) + 0.25 * _lastVal + currentRMS;

    //save last values
    _secondLastVal = _lastVal;
    _lastVal = currentRMS;
    //adjust the maximum value very slowly to counter volume decreasing (if sb makes the music more quiet)
    _maxVal -= 0.0001 * currentRMS;

    _trueRMSPeak -= 0.0005;
    if(_trueRMSPeak < currentRMS){
        _trueRMSPeak = currentRMS;
    }

    float cSpeed;
    int height;
    //check if no audio is playing right now
    if(currentRMS < 0.01){
        height = 0;
        //color speed factor
        cSpeed = 0.01 * SPEED_MULTIPLIER;
    }else{
        //check if this is a new record in volume
        if(currentRMS > _maxVal){
            _maxVal = currentRMS;
        }
        //calculate the next height
        height = (int) (LED_AMOUNT * (currentRMS / _maxVal)); // LED_COUNT
        //color speed factor
        cSpeed = (currentRMS / _maxVal) * SPEED_MULTIPLIER;
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
        _red[i] = (char) round(_r);
        _green[i] = (char) round(_g);
        _blue[i] = (char) round(_b);
    }
    //fill black space
    for(int i = height; i < LED_AMOUNT; i++){
        _red[i] = 0;
        _green[i] = 0;
        _blue[i] = 0;
    }
    int peak_height = std::max(std::min((int) (LED_AMOUNT * (_trueRMSPeak / _maxVal)), LED_AMOUNT - 1), 0); // LED_COUNT
    //cout<<peak_height<<" pH"<<endl;
    //set the peak point
    _red[peak_height] = (char) 255 - round(_r);
    _green[peak_height] = (char) 255 - round(_g);
    _blue[peak_height] = (char) 255 - round(_r);
    //now send the data
    _network->sendData(_red, _green, _blue);
}
