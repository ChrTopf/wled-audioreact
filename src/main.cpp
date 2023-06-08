#include <iostream>
#include <chrono>
#include <thread>
#include "Config.h"
#include "Log.h"
#include "SignalController.h"
#include "MaxVolumeEffect.h"

#define VERSION 0.1
#define CONFIG_FILE "../settings.json"

using namespace std;

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
    auto adr = config.getValues("addressees");
    for(int i = 0; i < adr.size(); i++){
        stringstream ss;
        ss << "Streaming to address '" << adr[i] << "'";
        Log::i(ss.str());
    }
    //create the signal controller
    SignalController controller(adr);

    //TODO: maybe select an audio stream???

    //TODO: remove the following testing code

    controller.setEffect(new MaxVolumeEffect());
    if(controller.startStreaming()){
        Log::i("Initialization complete. Started streaming.");
    }

    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    controller.stopStreaming();

    return 0;
}
