#include <iostream>
#include <chrono>
#include <signal.h>
#include "Config.h"
#include "Log.h"
#include "SignalController.h"

#define VERSION 0.5
#define CONFIG_FILE "../settings.json"
#define AUDIO_STREAM_INDEX_KEY "audioStream"
#define LED_AMOUNT_KEY "ledAmount"

using namespace std;

void onSegfault(int signal, siginfo_t *si, void *arg)
{
    stringstream ss;
    ss << "Caught segfault at address " << si->si_addr << ". Deleted audio stream setting." << endl;
    Log::e(ss.str());
    Config config(CONFIG_FILE);
    config.setString(AUDIO_STREAM_INDEX_KEY, "");
    exit(1);
}

int main() {
    std::srand( time(NULL));
    //print the name of the application
    cout <<
    "__        ___     _____ ____       _             _ _             ____                 _   \n"
    "\\ \\      / / |   | ____|  _ \\     / \\  _   _  __| (_) ___       |  _ \\ ___  __ _  ___| |_ \n"
    " \\ \\ /\\ / /| |   |  _| | | | |   / _ \\| | | |/ _` | |/ _ \\ _____| |_) / _ \\/ _` |/ __| __|\n"
    "  \\ V  V / | |___| |___| |_| |  / ___ \\ |_| | (_| | | (_) |_____|  _ <  __/ (_| | (__| |_ \n"
    "   \\_/\\_/  |_____|_____|____/  /_/   \\_\\__,_|\\__,_|_|\\___/      |_| \\_\\___|\\__,_|\\___|\\__|"
    << endl;
    cout << "Version: " << VERSION << endl;
    cout << "Copyright (c) 2023 ChrTopf & TheMaffinStuffer" << endl;
    //enable colored log messages
    Log::enableColor(true);
    //read the config file
    Config config(CONFIG_FILE);
    //set the log level
    int logLevel = config.getInt("loglevel", 1);
    Log::setLogLevel(logLevel);

    //do the startup sequence
    Log::i("Intializing...");
    //get all addressees
    auto addresses = config.getValues("addressees");
    for(int i = 0; i < addresses.size(); i++){
        stringstream ss;
        ss << "Streaming to address '" << addresses[i] << "'";
        Log::i(ss.str());
    }
    //get the length of the LED stripe
    int ledAmount = config.getInt(LED_AMOUNT_KEY, 144);
    if(ledAmount < 0){
        ledAmount *= -1;
    }
    EffectParameters::LED_AMOUNT = ledAmount;

    //catch segmentation fault
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = onSegfault;
    sa.sa_flags   = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);

    //create the signal controller
    SignalController controller(addresses, ledAmount, config);
    //let the user choose the audio stream
    controller.chooseAudioStream();
    //choose an effect
    controller.chooseEffect();
    //start streaming
    controller.startStreaming();

    bool exit = false;
    while(!exit){
        //print usage information
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << "Choose an action by its index:" << endl;
        cout << "[0]: Exit the application." << endl;
        cout << "[1]: Change effect." << endl;
        cout << "[2]: Switch audio source." << endl;
        //let the user decide
        int index = -1;
        while(true) {
            //get the index
            cout << "> ";
            cin >> index;
            //check if the index has been incorrect
            if(index < 0 || index > 2){
                Log::w("Please enter a valid index! Try again:");
            }else{
                break;
            }
        }
        //process the user input
        switch (index) {
            case 0:
                exit = true;
                break;
            case 1:
                controller.userSetNewEffectIndex();
                Log::i("The effect has been changed.");
                break;
            case 2:
                controller.stopStreaming();
                controller.userSetNewAudioIndex();
                controller.startStreaming();
                break;
        }
    }

    controller.stopStreaming();
    Log::i("Goodbye!");
    return 0;
}
