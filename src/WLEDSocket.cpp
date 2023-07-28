//
// Created by chrtopf on 04.06.23.
//

#include "WLEDSocket.h"

WLEDSocket::WLEDSocket(const std::string &address, const int ledAmount) {
    this->address = address;
    dataLength = 2+ledAmount*3;
    data = new char8_t [dataLength];
}

WLEDSocket::~WLEDSocket() {
    delete[] data;
    delete sock;
    delete ioContext;
}

bool WLEDSocket::initialize() {
    try{
        //prepare the socket
        ioContext = new asio::io_context();
        sock = new asio::ip::udp::socket(*ioContext, asio::ip::udp::v4());
        //resolve the destination address
        asio::ip::udp::resolver resolver(*ioContext);
        asio::ip::udp::resolver::results_type endpoints = resolver.resolve(asio::ip::udp::v4(), address, WLED_PORT);
        //check if the address was correct
        if(endpoints.empty()){
            std::stringstream ss;
            ss << "The IPv4 destination address '" << address << "' is invalid.";
            Log::e(ss.str());
            return false;
        }
        //get the first resolved host
        destination = *endpoints.begin();
    }catch (std::exception &e){
        std::stringstream ss;
        ss << "Socket could not be initialized. Error: " << e.what();
        Log::e(ss.str());
        return false;
    }
    return true;
}

bool WLEDSocket::sendData(const char8_t *red, const char8_t *green, const char8_t *blue) {
    //set the protocol to DRGB
    data[0] = 0x02;
    //set the number of seconds to wait for more packets before returning to normal mode
    data[1] = 0x02;
    //parse the rgb values
    int counter = 0;
    for(int i = 2; i < dataLength; i += 3){
        data[i] = red[counter];
        data[i + 1] = green[counter];
        data[i + 2] = blue[counter];
        counter++;
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
    for(int i = 2; i < dataLength; i += 3){
        data[i] = red;
        data[i + 1] = green;
        data[i + 2] = blue;
    }
    //send the data
    return send();
}

void WLEDSocket::close() {
    sock->close();
}

bool WLEDSocket::send() {
    //send the data
    std::size_t bytesSent = sock->send_to(asio::buffer(data, dataLength), destination);
    if (bytesSent == -1) {
        Log::e("Could not send data to '" + address + "'. Check your network connection!");
        return false;
    }
    return true;
}
