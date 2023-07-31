//
// Created by chrtopf on 04.06.23.
//

#ifndef WLED_AUDIOREACT_WLEDSOCKET_H
#define WLED_AUDIOREACT_WLEDSOCKET_H
#include <string>
#include <sstream>
#include <asio.hpp>
#include "Log.h"

#define WLED_PORT "21324"

class WLEDSocket {
private:
    asio::io_context *ioContext;
    asio::ip::udp::socket *sock;
    asio::ip::basic_endpoint<asio::ip::udp> destination;
    std::string address;

    char8_t *data;
    int dataLength;
    bool send();
    WLEDSocket(const WLEDSocket &original){};
public:
    WLEDSocket(const std::string &address, int ledAmount);
    ~WLEDSocket();
    bool initialize();
    bool sendData(const char8_t *red, const char8_t *green, const char8_t *blue);
    bool sendMonoData(int red, int green, int blue);
    void close();
};


#endif //WLED_AUDIOREACT_WLEDSOCKET_H
