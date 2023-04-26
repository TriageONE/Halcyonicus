//
// Created by Triage on 4/25/2023.
//

#ifndef HALCYONICUS_HALNETP_H
#define HALCYONICUS_HALNETP_H
#include <enet/enet.h>
#include <cstdio>

class HALNET_P{
    /** describes the idea of a player connecting to a server, and will evolve to become massive and interesting
     *
     */
    ENetAddress address {};
    ENetHost * client {};
    ENetPeer * peer;

public:
    HALNET_P(ENetAddress address){
        this->address = address;
        this->address.port = 1982;
        client = enet_host_create (nullptr /* create a client host */,
                                   1 /* only allow 1 outgoing connection */,
                                   2 /* allow up 2 channels to be used, 0 and 1 */,
                                   0 /* assume any amount of incoming bandwidth */,
                                   0 /* assume any amount of outgoing bandwidth */);
        if (client == nullptr) {
            fprintf (stderr,"An error occurred while trying to create an ENet client host.\n");
            exit (EXIT_FAILURE);
        }
    }

    int connect(){
        ENetEvent event;
        peer = enet_host_connect (client, & address, 2, 0);
        if (peer == NULL) {
            fprintf (stderr,"No available peers for initiating an ENet connection.\n");
            exit (EXIT_FAILURE);
        }
        /* Wait up to 5 seconds for the connection attempt to succeed. */
        if (enet_host_service (client, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
            puts ("Connection to Halcyonicus succeeded.");
        } else {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset (peer);
            puts ("Connection to Halcyonicus failed.");
        }
    }
};
#endif //HALCYONICUS_HALNETP_H
