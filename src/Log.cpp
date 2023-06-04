//
// Created by chrtopf on 04.06.23.
//

#include "Log.h"
#include <ctime>

bool Log::isColor = false;
LOG_LEVEL Log::logLevel = INFO;

void Log::out(const LOG_LEVEL level, const string& msg) {
    //check if the message of that level needs to be logged
    if(level < logLevel){
        return;
    }
    //get the current unix timestamp
    std::time_t now = std::time(NULL);
    std::tm * ptm = std::localtime(&now);
    char buffer[32];
    //format the timestamp
    std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
    //now print the log message
    if(isColor){
        cout << "[" << buffer << "][" << COLOR[level] << LABEL[level] << ANSI_RESET << "]: " << msg << endl;
    }else{
        cout << "[" << buffer << "][" << LABEL[level] << "]: " << msg << endl;
    }
}

void Log::enableColor(bool enabled) {
    isColor = enabled;
}

void Log::setLogLevel(LOG_LEVEL level) {
    logLevel = level;
}

void Log::setLogLevel(int level) {
    logLevel = static_cast<LOG_LEVEL>(level);
}
