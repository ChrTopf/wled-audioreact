//
// Created by chrtopf on 07.06.23.
//
#include "SignalController.h"
#include "effects/Effects.h"

#define AUDIO_STREAM_INDEX_KEY "audioStream"
#define SAMPLE_RATE_KEY "sampleRate"
#define EFFECT_INDEX_KEY "effectIndex"

SignalController::SignalController(const std::vector<std::string> &addressees, const int ledAmount, Config &config) : _network(NetworkHandler(addressees, ledAmount)),
                                                                                                       _config(config){
    _processor = AudioProcessor::getInstance();
    _blacklist = _config.getValues("streamBlacklist");
}

SignalController::~SignalController() {
    AudioProcessor::destroy();
}

void SignalController::chooseAudioStream() {
    //check if a default index has already been set
    if(!_config.keyExists(AUDIO_STREAM_INDEX_KEY) || !_config.keyExists(SAMPLE_RATE_KEY)){
        //let the user decide first
        Log::i("Please choose one of the following audio streams! (By entering its index)");
        //let the user set a new audio stream index
        userSetNewAudioIndex();
    }else{
        //get the last index from the config
        string name = _config.getString(AUDIO_STREAM_INDEX_KEY, "");
        double sampleRate = _config.getDouble(SAMPLE_RATE_KEY, 44100);
        //try to choose that index
        if(_processor->setAudioStreamByName(name) == 0 && _processor->setSampleRate(sampleRate)){
            stringstream ss;
            ss << "The previous stream with the name " << name << " and sample rate " << sampleRate << "Hz is no longer available. Please choose a different one!";
            Log::w(ss.str());
            //let the user set a new audio stream index
            userSetNewAudioIndex();
        }else{
            stringstream ss;
            ss << "Choosing last audio stream: '" << name << "'. Choosing last sample rate: " << sampleRate << "Hz.";
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
    bool success = _processor->start();
    std::this_thread::sleep_for(1000ms); //sleep a bit to wait for a potential segfault
    if(!success){
        Log::w("The audio stream setting has been reset.");
        //if processing has failed, reset the audio stream setting
        _config.setString(AUDIO_STREAM_INDEX_KEY, "");
        _config.saveConfig();
        return false;
    }
    //save the new settings here
    _config.saveConfig();
    return true;
}

void SignalController::stopStreaming() {
    _processor->stop();
    _network.closeAll();
}

void SignalController::userSetNewAudioIndex() {
    double sampleRate;
    //print all available indices
    vector<string> streamNames = _processor->printAudioStreams(_blacklist);
    //then let the user choose
    int index = -1;
    while(true){
        cout << "> ";
        cin >> index;
        //try to assign the index
        sampleRate = _processor->setAudioStreamByName(streamNames[index]);
        if(sampleRate > 0){
            break;
        }else{
            Log::w("Please enter a valid index! Try again:");
        }
    }
    //save the new index in the config
    _config.setString(AUDIO_STREAM_INDEX_KEY, streamNames[index]);

    Log::i("Please enter a sample size or 0 for the default one. Typical sample rates are 44100 or 48000.");
    //let the user choose
    double newSampleRate = 0;
    while(true){
        cout << "> ";
        cin >> newSampleRate;
        //check if the default sample rate was chosen and nothing needs to be changed
        if(newSampleRate == 0){
            newSampleRate = sampleRate;
            break;
        }else{
            if(_processor->setSampleRate(newSampleRate)){
                break;
            }else{
                Log::w("Please enter a valid sample rate between 60 and 400000! Try again:");
            }
        }
    }
    //save the sample rate in the config
    _config.setDouble(SAMPLE_RATE_KEY, newSampleRate);
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
    _config.saveConfig();
    //set the effect
    setEffect(effect);
}

void SignalController::setEffect(Effect *effect) {
    //assign a network handler to the effect
    effect->setNetworkHandler(&_network);
    //and set the effect
    _processor->setEffect(effect);
}
