//
// Created by chrtopf on 04.06.23.
//

#ifndef WLED_AUDIOREACT_NETWORKHANDLER_H
#define WLED_AUDIOREACT_NETWORKHANDLER_H
#include <iostream>
#include <string>
#include <vector>

//TODO: implement this class

class NetworkHandler {
public:
    NetworkHandler(const std::vector<std::string> &addressees);
    void sendData(const char red[144], const char green[144], const char blue[144]);
};


#endif //WLED_AUDIOREACT_NETWORKHANDLER_H
