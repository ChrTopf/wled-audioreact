//
// Created by chrtopf on 08.06.23.
//
#include "MaxVolumeEffect.h"
#include <cmath>

#define SPEED_MULTIPLIER 0.5

void MaxVolumeEffect::onData(const std::vector<float> &data) {
    //go through all the samples
    float currentMax = 0;
    for(int i = 0; i < data.size(); i++){
        float val = abs(data[i]);
        if(val > currentMax){
            currentMax = val;
        }
    }
    cout<<currentMax<<" sampleAbsMax | ";
    float currentRMS = 0;
    for(int i = 0; i < data.size(); i++){
        float val = data[i];
        currentRMS += val*val;
    }
    currentRMS = sqrt(currentRMS / ((float) data.size() / 2));
    cout<<currentRMS<<" sampleRMS | ";

    if(currentRMS == 0){
        _maxVal = 0;
        return;
    }
    currentRMS = log10(currentRMS)*20;
    cout<<currentRMS<<" dBm"<<endl;
    currentRMS = abs(1/currentRMS);

    currentMax = log10(currentMax)*20;
    cout<<currentRMS<<" dBm_max"<<endl;
    currentMax = abs(1/currentMax);

    //implement causal system with delay
    //currentRMS = 0.25 * _secondLastVal + 0.5 * _lastVal + currentRMS;
    //currentRMS = 0.5 * (_lastVal - currentRMS) + 0.25 * _lastVal + currentRMS;

    //save last values
    _secondLastVal = _lastVal;
    _lastVal = currentRMS;
    //adjust the maximum value very slowly to counter volume decreasing (if sb makes the music more quiet)
    _maxVal -= 0.0001 * currentRMS;

    float cSpeed;
    int height;
    //check if no audio is playing right now
    if(currentRMS < 0.01){
        height = 0;
        //color speed factor
        cSpeed = 0.01 * SPEED_MULTIPLIER;
    }else{
        //check if this is a new record in volume
        if(currentMax > _maxVal){
            _maxVal = currentMax;
        }
        //calculate the next height
        height = (int) (144 * (currentRMS / _maxVal)); // LED_COUNT
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
    int peak_height = max(min((int) (144 * (currentMax / _maxVal)), 143), 0); // LED_COUNT
    cout<<peak_height<<" pH"<<endl;
    red[peak_height] = (char) 255 - round(_r);
    green[peak_height] = (char) 255 - round(_g);
    blue[peak_height] = (char) 255 - round(_r);
    //now send the data
    _network->sendData(red, green, blue);
    Log::i("sent");
}
