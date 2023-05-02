//
// Created by Triage on 4/20/2023.
//

#ifndef HALCYONICUS_HALNET_H
#define HALCYONICUS_HALNET_H

#include <random>
#include "session.h"
#include "../crypto/ecdh.h"
#include "../crypto/cc20.h"
#include "../crypto/base64.h"
#include <iostream>
#include <enet/enet.h>
#include <set>
#include <tuple>
#include <map>
#include <ctime>   // Required for time() function
#include <chrono>
#include <random>



/**
 * HALNET is the backbone networking solution to everything halcyonicus.
 * Its an abstraction on top of UDP and works to patch all systems together.
 * This may turn into a massive class filled with thousands of working parts...
 */

class HALNET{

    /*
     *  Packet header construction should go as follows:
     *  Typical UDP headers present within packet,
     *  DST PORT 1982 for Player control
     *  DST PORT 1999 for Server control
     *
     *  In order to connect to a server, you must have proper authentication, which comes from HSV Blockchain, however this may be implemented down the line
     *  You must also utilize and support some sort of encryption for socket layer and transport layer security, CC20
     *
     * The scope of this library is to be a server and respond to players requests. We should, at the same time, develop HalNetP for players and how they should respond and call out
     *
     */

    ENetHost * server;
    ENetAddress address;

    bool listening = true;

    /**
     * Reserved words for the session ID:
     * CREQ = 1363497539
     * CASS = 1397965123 , Connection accepted, here is my public key
     * ARCN = 1313034817 , Already connected
     * CREJ = 1246057027 , Connect rejected
     */
    std::map<
        unsigned int, //Session identifier
        std::tuple<
            ENetAddress, //Their last IP and port
            unsigned int, //The last time they called out to us
            std::string //Their unshared secret
            >
    > sessions {};

    enum informalResponseCodes{
        CASS,   // Connection accepted, here is my public key
        ARCN,   // Already connected with session ID as next 4 bytes
        CREJ,   // Connect rejected, no extra data
        CHNG,   // Connection endpoint changed, recommend a reassociation
        FAIL,   // Test attempt failed, your string fed to test is invalid
        SCSS    // Success, what we did worked
    };

    bool initialized = false;

public:


    /**
     * Start a new server instance that listens on port 1982
     */
    HALNET(){
        // Bind to 0.0.0.0 for listen
        enet_address_set_host (& address, "127.0.0.2");
        // Bind address 1982 for player interactions
        address.port = 1982;

        if (!initialized)
        if (enet_initialize() != 0) {
            fprintf(stderr, "An error occurred while initializing ENet.\n");
            initialized = false;
            return;
        }
        initialized = true;


        std::cout << "SRV: Attempting to start server..." << std::endl;
        server = enet_host_create (&address /* the address to bind the server host to */,
                                   4      /* allow up to 4 clients and/or outgoing connections */, //TODO: make it so the server limit is this exactly
                                   2      /* allow up to 2 channels to be used, 0 and 1 */,
                                   0      /* assume any amount of incoming bandwidth */,
                                   0      /* assume any amount of outgoing bandwidth */);
        if (server == nullptr)
        {
            fprintf (stderr,
                     "SRV: An error occurred while trying to create an ENet server host.\n");
            exit (EXIT_FAILURE);
        }

        std::cout << "SRV: Server started on port " << address.port << std::endl;


    }

    ~HALNET() {
        enet_host_destroy(server);
    }

    void listen(){
        ENetEvent event;
        while (enet_host_service (server, &event, 3000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    printf ("SRV: A new client connected from %x:%u.\n",
                            event.peer -> address.host,
                            event.peer -> address.port);
                    /* Store any relevant client information here. */

                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    /* Clean up the packet now that we're done using it. */
                    processIncoming(&event);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf ("SRV: %s disconnected.\n", event.peer -> data);
                    /* Reset the peer's client information. */
                    event.peer -> data = nullptr;
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
            enet_packet_destroy(event.packet);
        }
    }

    void processIncoming(ENetEvent * event){
        std::string eventData(event->packet->dataLength, '\0');
        ::memcpy(eventData.data(), event->packet->data, event->packet->dataLength);
        /**
         * Halnet formula indicates that the first 4 characters of the data stream should be its command, followed by a verificator and then a bracketed payload of data.
         * for the connection request inbound, we would not use the verificator, rather we would instead look for key association data 
         * If its not a connection attempt, we should instead attempt to decrypt and decompress the data 
         */

        if (eventData.length() < 4){
            std::cout << "Received short packet, discarding.." << std::endl;
            return;
        }

        std::string sid = eventData.substr(0, 4);

        //If the session ID seems to be a CREQ,
        if (sid == "CREQ"){
            std::cout << "Recieved CREQ.." << std::endl;
            if (eventData.length() != 70) {
                std::cerr << "ERROR: Length of KEX CREQ seems to be shorter than 70, LEN=" << eventData.length() << std::endl << "\tPAYLOAD: " << eventData << std::endl;
                return;
            }
            //Let's look to see if there are any hosts by this IP in our session list first
            enet_uint32 host = event->peer->address.host;
            enet_uint16 port = event->peer->address.port;
            for (std::pair<unsigned int, std::tuple< _ENetAddress, unsigned int, std::string> > s : sessions){
                enet_uint32 host2 = get<0>(s.second).host;

                if (host == host2){
                    enet_uint16 port2 = get<0>(s.second).port;
                    if (port != port2) continue;
                    //Ports and host ID were the same, this is a problem and we should report it
                    // We have to assume anyone on another port with the same IP is possibly on a university network with other people
                    std::cerr << "ERROR: Client is already connected, sending ARCN<ID>..." << std::endl;
                    // Respond with "Already connected with session ID <id>
                    char sessionKeyChar[4];
                    ::memcpy(sessionKeyChar, &s.first, 4);
                    sendUnreliableResponse(event, formulateInfoResponse(ARCN, std::string{ sessionKeyChar } ) );
                    return;
                }
            }
            //Ok so nobody with that IP has already connected, they should have provided a public ECDH key after their CREQ now so we need to read that
            // Its always 32 bytes, and we just read through 4 of them, so we should start at place 4 and read 32 but make sure that the length is exactly 36

            std::string othersPubKey = eventData.substr(4, 65);
            ECDH e{};
            e.createNewKey();
            std::string ourKey = e.getPublicKey();

            //New association request
            bool invalid = true;
            unsigned int rSessionKey;
            while(invalid){

                rSessionKey = randomPositiveNumber();

                //For every session open so far,
                auto it = sessions.find(rSessionKey);
                if (it == sessions.end()){
                    //We found the end, therefore the session key did not exist
                    invalid = false;
                }
            }

            //Add the data in as a new session
            //Sessions require a session ID, IP address and port, last-heard from and a secret key
            auto now = std::chrono::system_clock::now();
            auto timestamp_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
            std::tuple<
                    ENetAddress, //Their last IP and port
                    unsigned int, //The last time they called out to us
                    std::string //Their unshared secret
            > session {
                event->peer->address,
                static_cast<unsigned int>(timestamp_seconds.count()),
                e.getPrivateKey(othersPubKey)
            };

            sessions.insert(std::pair<unsigned int, std::tuple
                    <
                    ENetAddress, //Their last IP and port
                    unsigned int, //The last time they called out to us
                    std::string //Their unshared secret
                    >
                > {rSessionKey, session});
            std::string sessionString = pad(rSessionKey);
            ourKey.append(sessionString);
            // Formulate a response
            sendReliableResponse(event, formulateInfoResponse(CASS, ourKey));
        }
        else {
            //Process the rest of the request via assumption that whoever sent this data has a session and is requesting legitimate data
            if (eventData.length() < 12){
                std::cout << "SRV-ERROR: Received short, possibly encrypted packet, discarding.." << std::endl;
                return;
            }

            unsigned int sessionID = parseSessionID(eventData.substr(0, 8));

            //Find session ID and association data
            auto it = sessions.find(sessionID);
            if (it == sessions.end()){
                //No sessions found
                std::cout << "SRV-WARN: Searched for sessions labeled with ID " << sessionID << " and found nothing." << std::endl;
                return;
            }
            //Validate the packet incoming and hopefully decrypt it
            SESSION s{ it->first, get<2>(it->second), event->peer->address};
            std::string result = verifyAndDecryptIncoming(eventData, s, event);
            if (result.length() < 4) {
                std::cerr << "SRV-ERROR: Recieved short packet, possibly without command after decrypt, discarding..." << std::endl;
                return;
            }
            processPhaseTwo(result, event, s);
        }

    }

    static std::string addressToString(const ENetAddress& address)
    {
        std::ostringstream ss;
        char ip_str[46];

        enet_address_get_host_ip(&address, ip_str, sizeof(ip_str));

        ss << ip_str << ":" << address.port;

        return ss.str();
    }

    static std::string pad(unsigned int num){
        std::stringstream paddedNumber;
        char n = 8 - std::to_string(num).length() ; //N should be a positive whole below 8
        while (n > 0) {
            paddedNumber << "0";
            n--;
        } //TODO: Wrong side buddy
        paddedNumber << num;
        return paddedNumber.str();
    }

    static unsigned int parseSessionID(const std::string& in){
        unsigned int num = 0; // initialize unsigned int variable to 0
        std::stringstream ss(in); // create a stringstream object with the string of numbers
        ss >> num;
        return num;
    }

    static int randomPositiveNumber() {
        std::random_device rd; // create a random device to seed the generator
        std::mt19937 gen(rd()); // create a Mersenne Twister generator seeded with the random device
        std::uniform_int_distribution<> dist(0, 99999999); // create a uniform distribution between 0 and 99,999,999
        int randomNumber = dist(gen); // generate a random number using the generator and distribution
        return randomNumber;
    }

    void processPhaseTwo(const std::string& decryptedPayload, ENetEvent * event, SESSION session) {
        //The first 4 of the payload should always be a command, never continuation data
        std::string command = decryptedPayload.substr(0, 4);
        std::string payload = decryptedPayload.substr(4, decryptedPayload.length()-4);
        if (command == "CTES") {
            if (payload == "Hello, Halcyonicus!"){
                sendReliableResponse(event, formatPayload( formulateInfoResponse(SCSS, "Hello, Traveller!"), session));
                return;
            } else {
                std::cerr << "ERROR: Ping received, but plaintext data did not match the expected string! Recieved: " << payload << std::endl;
                sendReliableResponse(event, formatPayload( formulateInfoResponse(FAIL, ""), session));
                return;
            }
        } else {
            return;
        }
    }

    static unsigned int getUINTSessionFromString(const std::string& sessionKey) {
        return *(unsigned int*) sessionKey.substr(0, 4).data();
    }

    static std::string formulateInfoResponse(HALNET::informalResponseCodes code, const std::string& data){
        std::stringstream ss{};
        switch (code) {
            case CASS:
                // Hand off the public key to the client
                ss << "CASS" << data;
                break;
            case ARCN:
                ss << "ARCN" << data;
                break;
            case CREJ:
                ss << "CREJ";
                break;
            case CHNG:
                ss << "CHNG";
                break;
            case SCSS:
                ss << "SCSS" << data;
                break;
            case FAIL:
                ss << "FAIL";
                break;
        }

        return ss.str();
    }

    /**
     * Encrypts and formats the payload so the server can correctly intake and decode it
     * @param payload the payload you wish to have encrypted
     * @return the formatted payload with a properly prepended session ID, nonce, and payload
     */
    static std::string formatPayload(const std::string& payload, SESSION session){

        std::stringstream ss;

        ss << session.getSessionKey();

        CC20 cc20{session.getPrivateKey()};
        cc20.setRandomNonce();
        std::string nonce = cc20.getNonce();
        cc20.setNonce("");

        ss << cc20.encryptMessage(nonce);

        cc20.setNonce(nonce);
        ss << cc20.encryptMessage(payload);

        return ss.str();
    }

    std::string verifyAndDecryptIncoming(const std::string& payload, SESSION session, ENetEvent * event){
        //The minimum payload should be 4 bytes (session), plus 8(nonce) to make 12, then another 4 (minimum actual payload) to make 16
        if (payload.length() < 20){
            std::cerr << "SRV-ERROR: Recieved short packet, returning null string, Payload: " << payload << std::endl;
            return {""};
        }

        if (parseSessionID(payload.substr(0, 8)) != session.getSessionKey()){
            std::cerr << "SRV-ERROR: Session ID was not the same as the registered session ID! Payload: " << payload;
            return {""};
        }
        // If the hosts do not match or the ports do not match, we should send a response that indicates they should re-associate
        if (session.getAddress().host != event->peer->address.host || session.getAddress().port != event->peer->address.port ){
            std::cerr << "SRV-ERROR: Host and port do not match," << std::endl
                    << "\tOUR_HOST: " << session.getAddress().host << std::endl
                    << "\tNEW_HOST: " << event->peer->address.host << std::endl
                    << "\tOUR_PORT: " << session.getAddress().port << std::endl
                    << "\tNEW_PORT: " << event->peer->address.port << std::endl;
            std::cerr << "SRV: Sending Reassociation request..." << std::endl;
            sendUnreliableResponse(event, formulateInfoResponse(CHNG, {""}));
            return {""};
        }

        CC20 cc20{session.getPrivateKey(), "00000000"};

        std::string nonce = payload.substr(8, 8);
        nonce = cc20.decryptMessage(nonce);

        cc20.setNonce(nonce);

        //Decrypt the rest of the payload and return that
        std::string data = cc20.decryptMessage(payload.substr(16, payload.length() - 17));
        return data;
    }

    // Communicates on channel 0 for reliable responses that can take extra time
    void sendReliableResponse(ENetEvent * event , const std::string& response ){
        ENetPacket* response_packet = enet_packet_create(response.data(), response.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(event->peer, 1, response_packet);
        enet_host_flush(server);
        enet_packet_destroy(response_packet);
    }

    // Communicates on channel 1, the unreliable channel for informational responses
    void sendUnreliableResponse(ENetEvent * event , const std::string& response ){
        ENetPacket* response_packet = enet_packet_create(response.data(), response.length() + 1, ENET_PACKET_FLAG_UNSEQUENCED);
        enet_peer_send(event->peer, 1, response_packet);
        enet_host_flush(server);
        enet_packet_destroy(response_packet);
    }



};
#endif //HALCYONICUS_HALNET_H
