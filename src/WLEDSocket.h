//
// Created by chrtopf on 04.06.23.
//

#ifndef WLED_AUDIOREACT_WLEDSOCKET_H
#define WLED_AUDIOREACT_WLEDSOCKET_H
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Log.h"

#define WLED_PORT 21324

class WLEDSocket {
private:
    std::string address;
    int sock;
    sockaddr_in destination;
    char data[2+144*3] = {0};
    bool send();
public:
    WLEDSocket(const std::string &address);
    bool initialize();
    bool sendData(const char red[144], const char green[144], const char blue[144]);
    bool sendRandomData();
    bool sendMonoData(const int red, const int green, const int blue);
    void close();
};


#endif //WLED_AUDIOREACT_WLEDSOCKET_H
