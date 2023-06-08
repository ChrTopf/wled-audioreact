//
// Created by chrtopf on 08.06.23.
//

#include "MaxVolumeEffect.h"

void MaxVolumeEffect::onData(const std::vector<float> &data) {
    //go through all the samples
    float currentMax = 0;
    for(int i = 0; i < data.size(); i++){
        float val = abs(data[i]);
        if(val > currentMax){
            currentMax = val;
        }
    }
    int height;
    //check if no audio is playing right now
    if(currentMax < 0.01){
        height = 0;
    }else{
        //check if this is a new record in volume
        if(currentMax > _maxVal){
            _maxVal = currentMax;
        }
        //calculate the next height
        height = (int) (144 * (currentMax / _maxVal));
    }

    //calculate the next color
    if(_r > 0 && _b == 0){
        _r--;
        _g++;
    }
    if(_g > 0 && _r == 0){
        _g--;
        _b++;
    }
    if(_b > 0 && _g == 0){
        _r++;
        _b--;
    }
    //calculate the new height
    for(int i = 0; i < height; i++){
        red[i] = _r;
        green[i] = _g;
        blue[i] = _b;
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
