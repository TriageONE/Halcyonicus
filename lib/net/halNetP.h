//
// Created by Triage on 4/25/2023.
//

#ifndef HALCYONICUS_HALNETP_H
#define HALCYONICUS_HALNETP_H
#include <enet/enet.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <thread>
#include "../crypto/cc20.h"


class HALNET_P{
    /** describes the idea of a player connecting to a server, and will evolve to become massive and interesting
     *
     */
    ENetAddress address {};
    ENetHost * client {};
    ENetPeer * server;

    ECDH ecdh{};

    unsigned int sessionID;
    std::string sessionSecret;

    bool connected = false;
    bool authenticated = false;

    bool expectingAssociation = false;
    unsigned int expectingHost = 0;

    // Sequence ID, < tries so far, and timestamp >
    std::mutex expectationMutex;
    std::map<unsigned int, std::pair<char, unsigned int>> expectations = {};


public:

    HALNET_P(const std::string& addressString){

        enet_address_set_host (& address, addressString.c_str());
        this->address.port = 1982;
        client = enet_host_create (nullptr /* create a client host */,
                                   2 /* only allow 1 outgoing connection */,
                                   2 /* allow up 2 channels to be used, 0 and 1 */,
                                   0 /* assume any amount of incoming bandwidth */,
                                   0 /* assume any amount of outgoing bandwidth */);
        if (client == nullptr) {
            fprintf (stderr,"CLI: An error occurred while trying to create an ENet client host.\n");
            exit (EXIT_FAILURE);
        }
    }

    //we need a method to tick every second and check if the expectations are present and report on them
    void checkExpectations(){
        expectationMutex.lock();

        expectationMutex.unlock();
    }

    void connect(){
        ENetEvent event;
        server = enet_host_connect (client, & address, 2, 0);
        if (server == nullptr) {
            fprintf (stderr,"CLI: No available peers for initiating an ENet connection.\n");
            exit (EXIT_FAILURE);
        }
        /* Wait up to 5 seconds for the connection attempt to succeed. */
        if (enet_host_service (client, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
            puts ("CLI: Connection to Halcyonicus succeeded!");
            connected = true;
        } else {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset (server);
            puts ("CLI: Connection to Halcyonicus failed.");
        }
    }

    void associate() {

        std::cout << "CLI: Initiating new crypto key..." << std::endl;
        ecdh.createNewKey();
        std::cout << "CLI: Key made, creating new CONREQ with following key: " << std::endl << "\t" << ecdh.getPublicKey()  << std::endl;
        std::string conreq = createConnectionRequest(ecdh.getPublicKey());

        expectingAssociation = true;
        expectingHost = server->address.host;

        sendUnreliable(conreq);
        //wait for the server's public key and listen for the result
    }

    void disconnect(){
        if (!connected) {
            std::cout << "CLI: Client was not connected to any server!" << std::endl;
            return;
        }
        ENetEvent event;
        enet_peer_disconnect (server, 0);
            /* Allow up to 3 seconds for the disconnect to succeed
             * and drop any packets received packets.
             */
        while (enet_host_service (client, & event, 3000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    puts ("CLI: Disconnection succeeded.");
                    authenticated = false;
                    connected = false;
                    ecdh={};
                    return;
                case ENET_EVENT_TYPE_NONE:
                case ENET_EVENT_TYPE_CONNECT:
                    break;
            }
        }
            /* We've arrived here, so the disconnect attempt didn't */
            /* succeed yet.  Force the connection down.             */
        enet_peer_reset (server);
        authenticated = false;
        connected = false;
        ecdh={};
        enet_packet_destroy (event.packet);
    }

    //TODO: Implement senders queue
    void sendReliable(const std::string& message){
        /* Create a reliable packet of size 7 containing "packet\0" */
        if (!connected) {
            std::cout << "CLI: Client was not connected to any server!" << std::endl;
            return;
        }

        ENetPacket * packet = enet_packet_create (message.c_str(),
                                                  message.length() + 1,
                                                  ENET_PACKET_FLAG_RELIABLE);
            /* Send the packet to the peer over channel id 0. */
            /* One could also broadcast the packet by         */
            /* enet_host_broadcast (host, 0, packet);         */

        enet_peer_send(server, 0, packet);

            /* One could just use enet_host_service() instead. */
        enet_host_flush(client);
        enet_packet_destroy(packet);
    }

    //TODO: Implement senders queue , mutex on listener? Why does this work but unsequenced packets dont??
    void sendUnreliable(const std::string& message){
        /* Create a reliable packet of size 7 containing "packet\0" */
        if (!connected) {
            std::cout << "CLI: Client was not connected to any server!" << std::endl;
            return;
        }

        ENetPacket * packet = enet_packet_create (message.c_str(),
                                                  message.length() + 1,
                                                  ENET_PACKET_FLAG_RELIABLE);
        /* Send the packet to the peer over channel id 0. */
        /* One could also broadcast the packet by         */
        /* enet_host_broadcast (host, 0, packet);         */
        enet_peer_send(server, 0, packet);
        /* One could just use enet_host_service() instead. */
        enet_host_flush(client);
        enet_packet_destroy (packet);
    }

    void listen() {
        if (!connected){
            std::cout << "Cannot listen for server if not connected!" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return;
        }
        ENetEvent event;
        while (enet_host_service (client, &event, 3000) > 0){
            switch (event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    processIncoming(&event);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("CLI: Disconnected from server\n");
                    // Handle disconnection here
                    connected = false;
                    break;

                case ENET_EVENT_TYPE_NONE:
                    printf("CLI: No event received\n");
                    break;

                case ENET_EVENT_TYPE_CONNECT:
                    printf("CLI: Connect event received?\n");
                    break;
            }
            enet_packet_destroy(event.packet);
        }
    }

    void processIncoming(ENetEvent * event){
        std::string eventData(event->packet->dataLength, '\0');
        ::memcpy(eventData.data(), event->packet->data, event->packet->dataLength);
        if (eventData.length() < 4){
            std::cout << "Received short packet, discarding.." << std::endl;
            return;
        }
        std::string sid = eventData.substr(0, 4);

        if (sid == "CASS") {
            //Were we expecting another connection accept?
            if (!expectingAssociation){
                char ip_address[256];
                enet_address_get_host_ip(&event->peer->address, ip_address, sizeof(ip_address));
                std::cerr << "ERROR: Unexpected association acceptance response found from " << ip_address << std::endl;
                return;
            }

            if (expectingHost != event->peer->address.host){
                std::cout << "CLI: Host was not the same as what we expected! Expected " << expectingHost << ", got " << event->peer->address.host << std::endl;
                return;
            }
            // Verify the host and port are valid
            if (!isHostAndPortValid(event)){
                std::cerr << "ERROR: Detected that source of CASS event was different than configured, possible MITM? Discarding packet." << std::endl;
                return;
            }

            std::string key, session;
            key = eventData.substr(4, 65);
            session = eventData.substr(69, 8);
            unsigned int sessionInt = parseID(session);
            this->sessionID = sessionInt;
            this->sessionSecret = ecdh.getPrivateKey(key);
            this->expectingAssociation = false;

        } else if( sid == "ARCN") {

            if (!expectingAssociation){
                char ip_address[256];
                enet_address_get_host_ip(&event->peer->address, ip_address, sizeof(ip_address));
                std::cerr << "CLI-ERROR: Unexpected association ARCN response found from " << ip_address << std::endl;
                return;
            }

            if (expectingHost != event->peer->address.host){
                std::cout << "CLI-ERROR: Host was not the same as what we expected! Expected " << expectingHost << ", got " << event->peer->address.host << std::endl;
                return;
            }
            // Verify the host and port are valid
            if (!isHostAndPortValid(event)){
                std::cerr << "CLI-ERROR: Detected that source of ARCN event was different than configured, possible MITM? Discarding packet." << std::endl;
                return;
            }

            std::string payload =  eventData.substr(4);
            std::cout << "CLI-ERROR: Recieved ARCN from server in plaintext, according to the server, the connection was already established with SID: \"" << eventData.substr(4)  << "\"" << std::endl;

            if (payload.length() < 8){
                std::cout << "CLI-ERROR: ARCN from server appended data too short to parse, possible corruption?" << std::endl;
                return;
            }


            //TODO: send deauth "RASC"

        } else {
            std::string decryptedPayload = verifyAndDecryptIncoming(eventData, event);
            processPhaseTwo(decryptedPayload, event);
        }
    }

    void processPhaseTwo(const std::string& decryptedPayload, ENetEvent * event){

        if (decryptedPayload.length() < 12) {
            std::cout << "Recieved short packet at phase 2, discarding.." << std::endl;
            return;
        }

        std::string command = decryptedPayload.substr(0, 4);
        std::string sequenceNumber = decryptedPayload.substr(4, 8);
        std::string payload = decryptedPayload.substr(12);

        if (command == "SCSS"){
            // This is the success vector, showing that we were sucessful in sending the ping that we had initially sent
            // How do we know what sequence number we sent was the one we wanted? we should have a way of knowing that we sent a test
            if (payload == "Hello, Traveller!"){
                std::cout << "CLI: Recieved successful ping back to client, sequence " << sequenceNumber << std::endl;
                satisfyExpectation(parseID(sequenceNumber));
            } else {
                std::cout << "CLI: Recieved failed ping back to client, sequence " << sequenceNumber << ", payload: " << payload << std::endl;
                satisfyExpectation(parseID(sequenceNumber));
            }
            return;
        } else if (command == "FAIL"){
            std::cout << "CLI-ERROR: Fail sent for ping sequence " << sequenceNumber << ", possible corruption?" << std::endl;
            return;
        }
        //TODO: Get this done. We need a way to know
    }


    static unsigned int parseID(const std::string& in){
        unsigned int num = 0; // initialize unsigned int variable to 0
        std::stringstream ss(in); // create a stringstream object with the string of numbers
        ss >> num;
        return num;
    }

    static unsigned int randomPositiveNumber() {
        std::random_device rd; // create a random device to seed the generator
        std::mt19937 gen(rd()); // create a Mersenne Twister generator seeded with the random device
        std::uniform_int_distribution<> dist(0, 99999999); // create a uniform distribution between 0 and 99,999,999
        int randomNumber = dist(gen); // generate a random number using the generator and distribution
        return randomNumber;
    }

    static std::string pad(unsigned int num){
        std::stringstream paddedNumber;
        char n = 8 - std::to_string(num).length() ; //N should be a positive whole below 8
        while (n > 0) {
            paddedNumber << "0";
            n--;
        }
        paddedNumber << num;
        return paddedNumber.str();
    }

    static std::string createConnectionRequest(const std::string& pubKey){
        std::stringstream ss;
        ss << "CREQ" << pubKey;
        return ss.str();
    }

    static long long current_time_ms()
    {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    }

    int sendTest(){
        if (!connected) {
            puts("CLI-ERROR: Test cannot be sent if you are not connected to HALNET!");
            return -1; //-1 means not connected to HALNET
        }
        unsigned int unique = randomPositiveNumber();
        std::string paddedUnique = pad(unique);
        std::stringstream ss;
        ss << "CTES" << paddedUnique << "Hello, Halcyonicus!";
        std::string payload = formatPayload(ss.str());

        createExpectation(unique);

        sendUnreliable(payload);
        // The scope of this does not require us to handle the rest of the test here. This is just a wrapper for sending the packet
        return 0;
    }

    void sendDissociation(){
        if (!connected) {
            puts("CLI-ERROR: Test cannot be sent if you are not connected to HALNET!");
            return;
        }
        unsigned int unique = randomPositiveNumber();
        std::string paddedUnique = pad(unique);
        std::stringstream ss;
        ss << "DISS" << paddedUnique;
        std::string payload = formatPayload(ss.str());

        createExpectation(unique);

        sendUnreliable(payload);
    }

    void createExpectation(const unsigned int sequence){
        expectationMutex.lock();
        try{
            expectations.insert(
                    std::pair<unsigned int, std::pair<char, unsigned int>> {
                            sequence, std::pair<char, unsigned int>{
                                    0, current_time_ms()
                            }
                    });
        }catch(const char *e){
            std::cerr << "CLI-ERROR: Caught exception during createExpectation: " << e << std::endl;
        }catch (...){
            std::cerr << "CLI-ERROR: Caught an unknown exception during createExpectation" << std::endl;
        }
        expectationMutex.unlock();
    }

    [[maybe_unused]] std::pair<char, unsigned int> satisfyExpectation(const unsigned int sequence){
        expectationMutex.lock();
        std::map <const unsigned int, std::pair<char, unsigned int>>::iterator it {};
        try{
            it = expectations.find(sequence);
            if (it == expectations.end()) {
                //We were not expecting this, so just return?
                return {-1, 0};
            }
        } catch(const char *e){
            std::cerr << "CLI-ERROR: Caught exception during satisfyExpectation: " << e << std::endl;
        } catch (...){
            std::cerr << "CLI-ERROR: Caught an unknown exception during satisfyExpectation" << std::endl;
        }
        expectationMutex.unlock();
        return it->second;
    }

    bool hasExpectation(unsigned int sequence) {
        bool has;
        expectationMutex.lock();
        try{
            has = expectations.contains(sequence);
        } catch(const char *e){
            std::cerr << "CLI-ERROR: Caught exception during hasExpectation: " << e << std::endl;
        } catch (...){
            std::cerr << "CLI-ERROR: Caught an unknown exception during hasExpectation" << std::endl;
        }
        expectationMutex.unlock();
        return has;
    }

    /**
     * Encrypts and formats the payload so the server can correctly intake and decode it
     * @param payload the payload you wish to have encrypted
     * @return the formatted payload with a properly prepended session ID, nonce, and payload
     */
    std::string formatPayload(const std::string& payload){
        if (!connected) {
            puts("ERROR: Payload cannot be formatted if you are not connected to HALNET!");
            return {""};
        }
        std::stringstream ss;

        ss << sessionID;

        CC20 cc20{this->sessionSecret};
        cc20.setRandomNonce();
        std::string nonce = cc20.getNonce();

        cc20.setNonce("00000000");
        ss << cc20.encryptMessage(nonce);

        cc20.setNonce(nonce);
        ss << cc20.encryptMessage(payload);

        return ss.str();
    }

    bool isHostAndPortValid(ENetEvent * event) {
        if (this->server->address.host != event->peer->address.host || this->server->address.port != event->peer->address.port ){
            std::cerr << "ERROR: Host and port do not match," << std::endl
                      << "\tOUR_HOST: " << this->server->address.host << std::endl
                      << "\tNEW_HOST: " << event->peer->address.host << std::endl
                      << "\tOUR_PORT: " << this->server->address.port << std::endl
                      << "\tNEW_PORT: " << event->peer->address.port << std::endl;
            std::cerr << "Sending Reassociation request..." << std::endl;
            //sendUnreliableResponse(event, formulateInfoResponse(CHNG, {""}));
            // We should find another method for this one
            return false;
        }
        return true;
    }

    /**
     * Returns the unencrypted payload of the recieved packet according to the parameters of this session
     * @param payload
     * @param event
     * @return
     */
    std::string verifyAndDecryptIncoming(const std::string& payload, ENetEvent * event){
        //The minimum payload should be 4 bytes (session), plus 8(nonce) to make 12, then another 4 (minimum actual payload) to make 16
        if (payload.length() < 16){
            std::cerr << "ERROR: Recieved short packet, returning null string, Payload: " << payload << std::endl;
            return {""};
        }
        unsigned int recvSessionID = parseID(payload.substr(0, 8));
        if (recvSessionID != sessionID){
            std::cerr << "ERROR: Session ID was not the same as the registered session ID! Payload: " << payload;
            return {""};
        }

        if (this->server->address.host != event->peer->address.host || this->server->address.port != event->peer->address.port ){
            std::cerr << "ERROR: Host and port do not match," << std::endl
                      << "\tOUR_HOST: " << this->server->address.host << std::endl
                      << "\tNEW_HOST: " << event->peer->address.host << std::endl
                      << "\tOUR_PORT: " << this->server->address.port << std::endl
                      << "\tNEW_PORT: " << event->peer->address.port << std::endl;
            std::cerr << "Sending Reassociation request..." << std::endl;
            //sendUnreliableResponse(event, formulateInfoResponse(CHNG, {""}));
            // We should find another method for this one
            return {""};
        }

        CC20 cc20{sessionSecret, ""};

        std::string nonce = payload.substr(8, 8);
        nonce = cc20.decryptMessage(nonce);
        cc20.setNonce(nonce);

        //Decrypt the rest of the payload and return that
        std::string data = cc20.decryptMessage(payload.substr(16));
        return data;
    }

    std::string addressToString(const ENetAddress& addr)
    {
        std::ostringstream ss;
        char ip_str[46];

        enet_address_get_host_ip(&address, ip_str, sizeof(ip_str));

        ss << ip_str << ":" << address.port;

        return ss.str();
    }

};
#endif //HALCYONICUS_HALNETP_H
