//
// Created by Triage on 4/23/2023.
//

#ifndef HALCYONICUS_CC20_H
#define HALCYONICUS_CC20_H

#include <sodium.h>
#include <iostream>

using namespace std;

class CC20{
    /**
     * ChaCha20, a bytewise symmetric encryption method that uses a 256 bit key and a 64 bit
     * nonce value to serially encrypt or decrypt a string of any size
     *
     */

    string key {};
    string nonce {};

    bool errored = false;
    bool nonceSet = false, keySet = false;

public:



    CC20() {
        if (sodium_init() == -1) {
            std::cerr << "Error initializing libsodium" << std::endl;
            this->errored = true;
        }
    }

    void setKey(string key){
        this->key = key;
    }

    void setNonce(string nonce) {
        this->nonce = nonce;
    }
};
#endif //HALCYONICUS_CC20_H
