//
// Created by chrtopf on 04.06.23.
//

#include "WLEDSocket.h"

WLEDSocket::WLEDSocket(const std::string &address) {
    this->address = address;
}

bool WLEDSocket::initialize() {
    //try to create the socket
    this->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        Log::e("Could not create the socket to '" + address + "'.");
        return false;
    }

    //create the structure for the destination address
    std::memset(&destination, 0, sizeof(destination));
    destination.sin_family = AF_INET;
    destination.sin_addr.s_addr = inet_addr(address.c_str());
    destination.sin_port = htons(WLED_PORT);

    return true;
}

bool WLEDSocket::sendData(const char *red, const char *green, const char *blue) {
    //set the protocol to DRGB
    data[0] = 0x02;
    //set the number of seconds to wait for more packets before returning to normal mode
    data[1] = 0x02;
    //parse the rgb values
    int counter = 0;
    for(int i = 2; i < sizeof(data); i += 3){
        data[i] = red[counter];
        data[i + 1] = green[counter];
        data[i + 2] = blue[counter];
        counter++;
    }
    //send the data
    return send();
}

bool WLEDSocket::sendRandomData() {
    //set the protocol to DRGB
    data[0] = 0x02;
    //set the number of seconds to wait for more packets before returning to normal mode
    data[1] = 0x02;
    //parse the rgb values
    for(int i = 2; i < sizeof(data); i += 3){
        data[i] = rand() % 255;
        data[i + 1] = rand() % 255;
        data[i + 2] = rand() % 255;
    }
    //send the data
    return send();
}

bool WLEDSocket::sendMonoData(const int red, const int green, const int blue) {
    //set the protocol to DRGB
    data[0] = 0x02;
    //set the number of seconds to wait for more packets before returning to normal mode
    data[1] = 0x02;
    //parse the rgb values
    for(int i = 2; i < sizeof(data); i += 3){
        data[i] = red;
        data[i + 1] = green;
        data[i + 2] = blue;
    }
    //send the data
    return send();
}

void WLEDSocket::close() {
    ::close(sock);
}

bool WLEDSocket::send() {
    //send the data
    ssize_t bytesSent = sendto(sock, data, sizeof(data), 0, (struct sockaddr*)&destination, sizeof(destination));
    if (bytesSent == -1) {
        Log::e("Could not send data to '" + address + "'");
        return false;
    }
    return true;
}
