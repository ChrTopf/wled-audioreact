//
// Created by chrtopf on 04.06.23.
//

#ifndef WLED_AUDIOREACT_NETWORKHANDLER_H
#define WLED_AUDIOREACT_NETWORKHANDLER_H
#include <iostream>
#include <string>
#include <vector>
#include "WLEDSocket.h"

class NetworkHandler {
private:
    const std::vector<std::string> &_addressees;
    std::vector<WLEDSocket*> _sockets;
    const int _ledAmount;
public:
    NetworkHandler(const std::vector<std::string> &addressees, int ledAmount);
    ~NetworkHandler();
    bool initializeAll();
    void sendData(const char8_t *red, const char8_t *green, const char8_t *blue);
    void closeAll();
};


#endif //WLED_AUDIOREACT_NETWORKHANDLER_H
