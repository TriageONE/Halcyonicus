//
// Created by Triage on 4/25/2023.
//

#ifndef HALCYONICUS_HALNETP_H
#define HALCYONICUS_HALNETP_H
#include <enet/enet.h>
#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

class HALNET_P{
    /** describes the idea of a player connecting to a server, and will evolve to become massive and interesting
     *
     */
    ENetAddress address {};
    ENetHost * client {};
    ENetPeer * server;
    bool connected = false;

public:
    HALNET_P(string addressString){
        enet_address_set_host (& address, addressString.c_str());
        this->address.port = 1982;
        client = enet_host_create (nullptr /* create a client host */,
                                   1 /* only allow 1 outgoing connection */,
                                   2 /* allow up 2 channels to be used, 0 and 1 */,
                                   0 /* assume any amount of incoming bandwidth */,
                                   0 /* assume any amount of outgoing bandwidth */);
        if (client == nullptr) {
            fprintf (stderr,"CLI: An error occurred while trying to create an ENet client host.\n");
            exit (EXIT_FAILURE);
        }
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
            puts ("CLI: Connection to Halcyonicus succeeded.");
            connected = true;
        } else {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset (server);
            puts ("CLI: Connection to Halcyonicus failed.");
        }
    }

    void disconnect(){
        if (!connected) {
            std::cout << "CLI: Client was not connected to any server!" << endl;
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
                    enet_packet_destroy (event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    puts ("CLI: Disconnection succeeded.");
                    return;
            }
        }
            /* We've arrived here, so the disconnect attempt didn't */
            /* succeed yet.  Force the connection down.             */
        enet_peer_reset (server);
        connected = false;
    }

    void send(std::string message){
        /* Create a reliable packet of size 7 containing "packet\0" */
        if (!connected) {
            cout << "CLI: Client was not connected to any server!" << endl;
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
    }
};
#endif //HALCYONICUS_HALNETP_H
