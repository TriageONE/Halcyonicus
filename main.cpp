#include "lib/crypto/ecdh.h"
#include <sodium.h>

#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

std::string stringToHex(const std::string& input) {
    std::stringstream stream;
    stream << std::hex << std::setfill('0');
    for (char i : input) {
        stream << /*" " <<*/ std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(i));
    }
    return stream.str();
}

int main() {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //Does not work for no good reason
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);
#endif

    ECDH alice{};
    ECDH bob{};

    alice.createNewKey();
    bob.createNewKey();

    string aEC = alice.extractECPOINT();
    string bEC = bob.extractECPOINT();

    string aliceSecret = alice.getSecretFromOthersKey(bEC);
    string bobSecret = bob.getSecretFromOthersKey(aEC);

    //You can wrap each string in stringToHex() if they pose issues with formatting
    cout << stringToHex(aEC) << endl << stringToHex(bEC) << endl;
    cout << stringToHex(aliceSecret) << endl << stringToHex(bobSecret) << endl;

    string alicesSecretMessage = "This is alice's secret message that must be encrypted and decrypted";
    string bobsSecretMessage = "This is bob's secret message that must be encrypted and decrypted";

    // Initialize libsodium
    if (sodium_init() == -1) {
        std::cerr << "Error initializing libsodium" << std::endl;
        return 1;
    }

    // Generate a random key and nonce
    unsigned char key[crypto_stream_chacha20_KEYBYTES];
    unsigned char nonce[crypto_stream_chacha20_NONCEBYTES];
    randombytes_buf(key, sizeof key);
    randombytes_buf(nonce, sizeof nonce);

    // Encrypt a string
    std::string plaintext = "This is a secret message";
    std::string ciphertext(plaintext.size(), '\0');
    crypto_stream_chacha20_xor(reinterpret_cast<unsigned char*>(&ciphertext[0]),
                               reinterpret_cast<const unsigned char*>(plaintext.c_str()),
                               plaintext.size(),
                               nonce,
                               key);

    // Decrypt the string
    std::string decryptedtext(ciphertext.size(), '\0');
    crypto_stream_chacha20_xor(reinterpret_cast<unsigned char*>(&decryptedtext[0]),
                               reinterpret_cast<const unsigned char*>(ciphertext.c_str()),
                               ciphertext.size(),
                               nonce,
                               key);

    // Print the results
    std::cout << "Original message: " << plaintext << std::endl;
    std::cout << "Encrypted message: " << ciphertext << std::endl;
    std::cout << "Decrypted message: " << decryptedtext << std::endl;

    return 0;

}
