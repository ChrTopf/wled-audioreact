//
// Created by chrtopf on 07.06.23.
//
#include "SignalController.h"
#include "effects/Effects.h"

#define AUDIO_STREAM_INDEX_KEY "audioStream"
#define EFFECT_INDEX_KEY "effectIndex"

SignalController::SignalController(const std::vector<std::string> &addressees, const Config &config) : _network(NetworkHandler(addressees)),
                                                                                                       _config(config){
    _processor = AudioProcessor::getInstance();
    _blacklist = _config.getValues("streamBlacklist");
}

SignalController::~SignalController() {
    AudioProcessor::destroy();
}

void SignalController::chooseAudioStream() {
    //check if a default index has already been set
    if(!_config.keyExists(AUDIO_STREAM_INDEX_KEY)){
        //let the user decide first
        Log::i("Please choose one of the following audio streams! (By entering its index)");
        //let the user set a new audio stream index
        userSetNewAudioIndex();
    }else{
        //get the last index from the config
        string name = _config.getString(AUDIO_STREAM_INDEX_KEY, "");
        //try to choose that index
        if(!_processor->setAudioStreamByName(name)){
            stringstream ss;
            ss << "The previous stream with the name " << name << " is no longer available. Please choose a different one!";
            Log::w(ss.str());
            //let the user set a new audio stream index
            userSetNewAudioIndex();
        }else{
            stringstream ss;
            ss << "Choosing last audio stream: '" << name << "'.";
            Log::i(ss.str());
        }
    }
}

void SignalController::chooseEffect() {
    //check if a default index has already been set
    if(!_config.keyExists(AUDIO_STREAM_INDEX_KEY)){
        //let the user decide what the first effect should be
        Log::i("Please choose one of the following effects! (By entering its index)");
        //let the user set a new effect
        userSetNewEffectIndex();
    }else{
        //get the last effect index from the config file
        int index = _config.getInt(EFFECT_INDEX_KEY, -1);
        Effect *effect = Effects::EFFECTS[index].second();
        //check if the effect was created
        if(effect == nullptr){
            stringstream ss;
            ss << "The previous effect with the index " << index << " is no longer available. Please choose a different one!";
            Log::w(ss.str());
            //let the user set a new effect
            userSetNewEffectIndex();
        }else{
            setEffect(effect);
            stringstream ss;
            ss << "Loaded last effect: '" << Effects::EFFECTS[index].first << "'.";
            Log::i(ss.str());
        }
    }
}

bool SignalController::startStreaming() {
    //check if the effect has already been set
    if(!_processor->hasEffect()){
        Log::e("Cannot start streaming. The effect has not been set yet.");
        return false;
    }
    //start the networking
    _network.initializeAll();
    //try to start processing
    if(!_processor->start()){
        Log::w("The audio stream setting has been reset.");
        //if processing has failed, reset the audio stream setting
        _config.setString(AUDIO_STREAM_INDEX_KEY, "");
        return false;
    }
    return true;
}

void SignalController::stopStreaming() {
    _processor->stop();
    _network.closeAll();
}

void SignalController::userSetNewAudioIndex() {
    //print all available indices
    vector<string> streamNames = _processor->printAudioStreams(_blacklist);
    //then let the user choose
    int index = -1;
    while(true){
        //get the index
        cout << "> ";
        cin >> index;
        //try to assign the index
        if(_processor->setAudioStreamByName(streamNames[index])){
            break;
        }else{
            Log::w("Please enter a valid index! Try again:");
        }
    }
    //save the new index in the config
    _config.setString(AUDIO_STREAM_INDEX_KEY, streamNames[index]);
}

void SignalController::userSetNewEffectIndex() {
    //print all effects
    for(int i = 0; i < Effects::EFFECTS.size(); i++){
        printEffect(i, Effects::EFFECTS[i].first);
    }
    //then let the user choose
    int index = -1;
    Effect *effect;
    while(true){
        //get the index
        cout << "> ";
        cin >> index;
        //check if the index is valid
        if(index >= 0 && index < Effects::EFFECTS.size()){
            //get an instance for the effect
            effect = Effects::EFFECTS[index].second();
            break;
        }else{
            Log::w("Please enter a valid index! Try again:");
        }
    }
    //safe index in config
    _config.setInt(EFFECT_INDEX_KEY, index);
    //set the effect
    setEffect(effect);
}

void SignalController::setEffect(Effect *effect) {
    //assign a network handler to the effect
    effect->setNetworkHandler(&_network);
    //and set the effect
    _processor->setEffect(effect);
}
