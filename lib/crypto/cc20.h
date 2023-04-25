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

    unsigned char key[crypto_stream_chacha20_KEYBYTES]{'\0'};
    unsigned char nonce[crypto_stream_chacha20_NONCEBYTES]{'\0'};

    bool errored = false;
    bool nonceSet = false, keySet = false;

public:

    CC20(const string& key, const string& nonce) {
        nonce.copy((char*) this->nonce, crypto_stream_chacha20_NONCEBYTES);
        key.copy((char*) this->key, crypto_stream_chacha20_KEYBYTES);
        this->keySet = true;
        this->nonceSet = true;
        if (sodium_init() == -1) {
            std::cerr << "Error initializing libsodium" << std::endl;
            this->errored = true;
        }
    }

    explicit CC20(const string& key) {
        setRandomNonce();
        key.copy((char*) this->key, crypto_stream_chacha20_KEYBYTES);
        this->keySet = true;
        if (sodium_init() == -1) {
            std::cerr << "Error initializing libsodium" << std::endl;
            this->errored = true;
            return;
        }
    }

    CC20(){
        if (sodium_init() == -1) {
            std::cerr << "Error initializing libsodium" << std::endl;
            this->errored = true;
            return;
        }
    }


    void setKey(const string& newKey){
        newKey.copy((char*) this->key, crypto_stream_chacha20_KEYBYTES);
        keySet = true;
    }

    void setNonce(const string& newNonce) {
        newNonce.copy((char*) this->nonce, crypto_stream_chacha20_NONCEBYTES);
        nonceSet = true;
    }

    void setRandomNonce() {
        randombytes_buf(nonce, sizeof nonce);
        nonceSet = true;
    }

    string getNonce() {
        if (this->nonceSet)
            return string{(char*)this->nonce};
        return string{""};
    }

    [[nodiscard]] bool isNonceSet() const { return this->nonceSet; }

    [[nodiscard]] bool isKeySet() const { return this->keySet; }

    [[nodiscard]] bool isErrored() const { return this->errored; }

    string encryptMessage(const string& message){
        if (!isKeySet() || !isNonceSet() || isErrored()){
            cerr << "ERROR WHEN PROCESSING ENCRYPT: KEYSET=" << isKeySet() << ", NONCESET=" << isNonceSet() << ", ERRORED=" << isErrored() << endl;
            return {""};
        }
        std::string ciphertext(message.size(), '\0');
        crypto_stream_chacha20_xor(reinterpret_cast<unsigned char*>(&ciphertext[0]),
                                   reinterpret_cast<const unsigned char*>(message.c_str()),
                                   message.size(),
                                   nonce,
                                   key);
        return ciphertext;
    }

    string decryptMessage(const string& ciphertext){
        if (!isKeySet() || !isNonceSet() || isErrored()){
            cerr << "ERROR WHEN PROCESSING ENCRYPT: KEYSET=" << isKeySet() << ", NONCESET=" << isNonceSet() << ", ERRORED=" << isErrored() << endl;
            return {""};
        }

        std::string message(ciphertext.size(), '\0');
        crypto_stream_chacha20_xor(reinterpret_cast<unsigned char*>(&message[0]),
                                   reinterpret_cast<const unsigned char*>(ciphertext.c_str()),
                                   ciphertext.size(),
                                   nonce,
                                   key);
        return message;
    }

};
#endif //HALCYONICUS_CC20_H
