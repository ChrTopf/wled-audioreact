//
// Created by chrtopf on 04.06.23.
//

#ifndef WLED_AUDIOREACT_WLEDSOCKET_H
#define WLED_AUDIOREACT_WLEDSOCKET_H
#include <string>
#include <cstring>
#if __has_include(<sys/socket.h>)
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <sys/types.h>
#endif
#include <unistd.h>
#include "Log.h"

#define WLED_PORT 21324

class WLEDSocket {
private:
    std::string address;
    int sock;
    sockaddr_in destination;
    char *data;
    int dataLength;
    bool send();
public:
    WLEDSocket(const std::string &address, int ledAmount);
    ~WLEDSocket();
    bool initialize();
    bool sendData(const char *red, const char *green, const char *blue);
    bool sendRandomData();
    bool sendMonoData(int red, int green, int blue);
    void close();
};


#endif //WLED_AUDIOREACT_WLEDSOCKET_H
