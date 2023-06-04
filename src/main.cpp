#include <iostream>
#include "Log.h"

using namespace std;

int main() {
    //print the name of the application
    cout <<
    "__        ___     _____ ____       _             _ _             ____                 _   \n"
    "\\ \\      / / |   | ____|  _ \\     / \\  _   _  __| (_) ___       |  _ \\ ___  __ _  ___| |_ \n"
    " \\ \\ /\\ / /| |   |  _| | | | |   / _ \\| | | |/ _` | |/ _ \\ _____| |_) / _ \\/ _` |/ __| __|\n"
    "  \\ V  V / | |___| |___| |_| |  / ___ \\ |_| | (_| | | (_) |_____|  _ <  __/ (_| | (__| |_ \n"
    "   \\_/\\_/  |_____|_____|____/  /_/   \\_\\__,_|\\__,_|_|\\___/      |_| \\_\\___|\\__,_|\\___|\\__|"
    << endl;
    cout << "Copyright (c) 2023 ChrTopf & TheMaffinStuffer" << endl;
    //TODO: do the startup sequence
    Log::enableColor(true);
    Log::setLogLevel(DEBUG);
    //test the log messages
    Log::d("This is a debug message.");
    Log::i("Here is an info message.");
    Log::w("Now this is not critical, but be warned!");
    Log::e("Something really bad just happend.");


    return 0;
}
