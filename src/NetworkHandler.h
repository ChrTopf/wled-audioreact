//
// Created by chrtopf on 04.06.23.
//

#ifndef WLED_AUDIOREACT_NETWORKHANDLER_H
#define WLED_AUDIOREACT_NETWORKHANDLER_H
#include <iostream>
#include <string>
#include <vector>
#include "WLEDSocket.h"

//TODO: implement this class

class NetworkHandler {
private:
    const std::vector<std::string> &_addressees;
    std::vector<WLEDSocket> _sockets;
public:
    NetworkHandler(const std::vector<std::string> &addressees);
    bool initializeAll();
    void sendData(const char red[144], const char green[144], const char blue[144]);
    void closeAll();
};


#endif //WLED_AUDIOREACT_NETWORKHANDLER_H
