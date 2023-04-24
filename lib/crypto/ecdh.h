//
// Created by Triage on 4/22/2023.
//

#ifndef HALCYONICUS_ECDH_H
#define HALCYONICUS_ECDH_H

#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ecdh.h>

#include <iostream>
#include <string>
#include <cassert>
#include <cstdio>
#include <vector>
#include <iomanip>

using namespace std;

class ECDH{
    /*
     * Elliptic Curve Diffie-Hellman
     *
     * Meant to organize a pre shared key between two endpoints that also use this software
     * The idea is that we can set a random key here for ourselves and then use it temporarily to negotiate an AES PSK for block cipher communications
     *
     */
    EC_KEY* key;
    bool errored;

public:

    ECDH() = default;

    ~ECDH() {
        OPENSSL_free(key);
    }

    void createNewKey()
    {
        EC_KEY* ecdh_key;
        ecdh_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        if (ecdh_key == nullptr)
        {
            std::cerr << "Error: Failed to create ECDH key." << std::endl;
            this->key = nullptr;
            this->errored = true;
            return;
        }
        if (EC_KEY_generate_key(ecdh_key) == 0)
        {
            std::cerr << "Error: Failed to generate ECDH key." << std::endl;
            EC_KEY_free(ecdh_key);
            this->key = nullptr;
            this->errored = true;
            return;
        }
        this->key = ecdh_key;
        this->errored = false;
    }

    std::string extractECPOINT() {
        /* Get the EC_GROUP object for the key
        const EC_GROUP *ecGroup = EC_KEY_get0_group(key);

        // Get the EC_POINT object for the public key
        const EC_POINT *ecPoint = EC_KEY_get0_public_key(key);

        // Create a new BIO object to store the public key in PEM format
        BIO *bio = BIO_new(BIO_s_mem());

        // Write the public key in PEM format to the BIO object
        if (!PEM_write_bio_EC_PUBKEY(bio, key)) {
            BIO_free(bio);
            return "";
        }

        // Read the public key from the BIO object into a std::string
        char *buffer = nullptr;
        long length = BIO_get_mem_data(bio, &buffer);
        std::string publicKey(buffer, length);

        // Free the BIO object
        BIO_free(bio);

        return publicKey;*/

        EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
        const EC_POINT* ec_point = EC_KEY_get0_public_key(key);

        // Get size of serialized point
        size_t size = EC_POINT_point2oct(group, ec_point, POINT_CONVERSION_UNCOMPRESSED, NULL, 0, NULL);

        // Serialize point to string
        std::string serialized_point(size, '\0');
        EC_POINT_point2oct(group, ec_point, POINT_CONVERSION_UNCOMPRESSED, (unsigned char*)serialized_point.data(), size, NULL);
        return serialized_point;
    }

    std::string getSecretFromOthersKey(const std::string &publicKey){
        EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
        EC_POINT *ec_point = EC_POINT_new(group);
        const auto* newString = reinterpret_cast<const unsigned char*>(publicKey.data());

        if (!EC_POINT_oct2point( group, ec_point, newString, publicKey.size(), nullptr)) {
            cerr << "ERROR OCCRUED DURING oct2point CONVERSION, STRING FED: " << publicKey << endl;
            return string{""};
        }
        const char * secret = get_secret(ec_point);
        return string{secret};
    }

private:

    const char *get_secret(const EC_POINT *peer_pub_key){
        int field_size;
        void *secret;

        field_size = EC_GROUP_get_degree(EC_KEY_get0_group(key));
        size_t secret_len = (field_size + 7) / 8;

        if (nullptr == (secret = OPENSSL_malloc((field_size + 7) / 8))) {
            printf("Failed to allocate memory for secret");
            return nullptr;
        }

        secret_len = ECDH_compute_key(secret, secret_len,
                                      peer_pub_key, key, nullptr);

        if (secret_len <= 0) {
            OPENSSL_free(secret);
            return nullptr;
        }
        return (const char*) (secret);
    }
};

#endif //HALCYONICUS_ECDH_H
