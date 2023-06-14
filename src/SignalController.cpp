//
// Created by chrtopf on 07.06.23.
//
#include "SignalController.h"
#include "MaxVolumeEffect.h"

SignalController::SignalController(const std::vector<std::string> &addressees) : _network(NetworkHandler(addressees)){
    _processor = AudioProcessor::getInstance();
}

SignalController::~SignalController() {
    AudioProcessor::destroy();
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
    _processor->start();
    return true;
}

void SignalController::stopStreaming() {
    _processor->stop();
    _network.closeAll();
}




