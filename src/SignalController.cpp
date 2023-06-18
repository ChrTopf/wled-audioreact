//
// Created by chrtopf on 07.06.23.
//
#include "SignalController.h"
#include "MaxVolumeEffect.h"

SignalController::SignalController(const std::vector<std::string> &addressees, const Config &config) : _network(NetworkHandler(addressees)),
                                                                                                       _config(config){
    _processor = AudioProcessor::getInstance();
}

SignalController::~SignalController() {
    AudioProcessor::destroy();
}

void SignalController::chooseAudioStream() {
    //TODO: check if a default index has already been set
    if(!_config.keyExists("audioStreamIndex")){
        //let the user decide first
        Log::i("Please choose one of the following audio streams, by their index!");
        //let the user set a new audio stream index
        userSetNewAudioIndex();
    }else{
        //get the last index from the config
        int index = _config.getInt("audioStreamIndex", 0);
        //try to choose that index
        if(!_processor->setAudioStreamIndex(index)){
            stringstream ss;
            ss << "The previous index " << index << " is no longer available. Please choose a different one!";
            Log::w(ss.str());
            //let the user set a new audio stream index
            userSetNewAudioIndex();
        }
    }
}

void SignalController::setEffect(Effect *effect) {
    //assign a network handler to the effect
    effect->setNetworkHandler(&_network);
    //and set the effect
    _processor->setEffect(effect);
}

bool SignalController::startStreaming() {
    //check if the effect has already been set
    if(!_processor->hasEffect()){
        Log::e("Cannot start streaming. The effect has not been set yet.");
        return false;
    }
    //start the networking
    _network.initializeAll();
    return _processor->start();
}

void SignalController::stopStreaming() {
    _processor->stop();
    _network.closeAll();
}

void SignalController::userSetNewAudioIndex() {
    //print all available indices
    _processor->printAudioStreams();
    //then let the user choose
    int index = -1;
    while(true){
        //get the index
        cout << "> ";
        cin >> index;
        //try to assign the index
        if(_processor->setAudioStreamIndex(index)){
            break;
        }else{
            Log::w("Please enter a valid index! Try again:");
        }
    }
    //save the new index in the config
    _config.setInt("audioStreamIndex", index);
}




