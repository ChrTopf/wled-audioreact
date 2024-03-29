//
// Created by chrtopf on 04.06.23.
//
#include "NetworkHandler.h"
#include <sstream>

NetworkHandler::NetworkHandler(const std::vector<std::string> &addressees, int ledAmount) : _addressees(addressees), _ledAmount(ledAmount){
    _sockets = std::vector<WLEDSocket*>();
}

NetworkHandler::~NetworkHandler() {
    for(auto *s : _sockets){
        delete s;
    }
    _sockets.clear();
}

bool NetworkHandler::initializeAll() {
    for(int i = 0; i < _addressees.size(); i++){
        auto *socket = new WLEDSocket(_addressees[i], _ledAmount);
        //try to create the socket
        if(!socket->initialize()){
            std::stringstream ss;
            ss << "The socket to the address '" << _addressees[i] << "' could not be initialized.";
            Log::e(ss.str());
            return false;
        }
        //add the socket to the internal list
        _sockets.push_back(socket);
    }
    return true;
}

void NetworkHandler::sendData(const char8_t *red, const char8_t *green, const char8_t *blue) {
    for(int i = 0; i < _sockets.size(); i++){
        //try to send data
        if(!_sockets[i]->sendData(red, green, blue)){
            _sockets[i]->close();
            //delete that socket connection
            _sockets.erase(std::next(_sockets.begin(), i));
        }
    }
}

void NetworkHandler::closeAll() {
    for(int i = 0; i < _sockets.size(); i++){
        _sockets[i]->close();
    }
    _sockets.clear();
}
