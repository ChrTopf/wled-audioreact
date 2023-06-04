#include <iostream>
#include <chrono>
#include <thread>
#include "Config.h"
#include "Log.h"
#include "WLEDSocket.h"

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

    //TODO: do the startup sequence
    //TODO: select an audio stream

    //TODO: remove the following testing code

    //test the log messages
    Log::d("This is a debug message.");
    Log::i("Here is an info message.");
    Log::w("Now this is not critical, but be warned!");
    Log::e("Something really bad just happend.");

    //get all addressees
    auto adr = config.getValues("addressees");
    for(int i = 0; i < adr.size(); i++){
        Log::d(adr[i]);
    }

    WLEDSocket conn("192.168.241.59");
    conn.initialize();

    int r=255,g=0,b=0;
    while (true){
        if(r > 0 && b == 0){
            r--;
            g++;
        }
        if(g > 0 && r == 0){
            g--;
            b++;
        }
        if(b > 0 && g == 0){
            r++;
            b--;
        }
        //
        conn.sendMonoData(r, g, b);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
