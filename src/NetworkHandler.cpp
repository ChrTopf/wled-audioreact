//
// Created by chrtopf on 04.06.23.
//
#include "NetworkHandler.h"
#include <sstream>

NetworkHandler::NetworkHandler(const std::vector<std::string> &addressees) : _addressees(addressees){
    _sockets = std::vector<WLEDSocket>();
}

bool NetworkHandler::initializeAll() {
    for(int i = 0; i < _addressees.size(); i++){
        WLEDSocket socket(_addressees[i]);
        //try to create the socket
        if(!socket.initialize()){
            stringstream ss;
            ss << "The socket to the address '" << _addressees[i] << "' could not be initialized.";
            Log::e(ss.str());
            return false;
        }
        //add the socket to the internal list
        _sockets.push_back(socket);
    }
    return true;
}

void NetworkHandler::sendData(const char *red, const char *green, const char *blue) {
    for(int i = 0; i < _sockets.size(); i++){
        //try to send data
        if(!_sockets[i].sendData(red, green, blue)){
            _sockets[i].close();
            //delete that socket connection
            _sockets.erase(std::next(_sockets.begin(), i));
        }
    }
}

void NetworkHandler::closeAll() {
    for(int i = 0; i < _sockets.size(); i++){
        _sockets[i].close();
    }
}
