//
// Created by Triage on 4/28/2023.
//

#ifndef HALCYONICUS_SESSION_H
#define HALCYONICUS_SESSION_H

#include <string>
#include <enet/enet.h>

class SESSION {

    /**
     * A session includes details about the user connected, such as IP and last used port, as well as a session key and a private key
     * there are a few reserved integers, mainly for command shaping and connection requests or reconnect requests
     *
     * CREQ = 1363497539
     */

    unsigned int sessionKey;
    ENetAddress address{};
    std::string privateKey;

public:
    /**
     * Sessions must be initialized with a private key and a session key
     */
    SESSION(unsigned int sessionKey, std::string privateKey, ENetAddress address){
        this->privateKey = std::move(privateKey);
        this->sessionKey = sessionKey;
        this->address = address;
    }

    std::string getPrivateKey(){return privateKey;}
    [[nodiscard]] unsigned int getSessionKey() const{return sessionKey;}
    ENetAddress getAddress(){return address;}

    void updateSessionKey(unsigned int newSession){this->sessionKey = newSession;}
    void updatePrivateKey(const std::string &newPrivate){this->privateKey = newPrivate;}
    void updateAddress(ENetAddress newAddress){this->address = newAddress;}

};

#endif //HALCYONICUS_SESSION_H
