//
// Created by Triage on 4/20/2023.
//

#ifndef HALCYONICUS_HALNET_H
#define HALCYONICUS_HALNET_H

#include <enet/enet.h>
#include <cstdio>

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
public:
    /**
     * Start a new server instance that listens on port 1982
     */
    HALNET(){
        // Bind to 0.0.0.0 for listen
        address.host = ENET_HOST_ANY;
        // Bind address 1982 for player interactions
        address.port = 1982;

        server = enet_host_create (&address /* the address to bind the server host to */,
                                   4096      /* allow up to 4096 clients and/or outgoing connections */,
                                   2      /* allow up to 2 channels to be used, 0 and 1 */,
                                   0      /* assume any amount of incoming bandwidth */,
                                   0      /* assume any amount of outgoing bandwidth */);
        if (server == NULL)
        {
            fprintf (stderr,
                     "An error occurred while trying to create an ENet server host.\n");
            exit (EXIT_FAILURE);
        }

    }

    ~HALNET() {
        enet_host_destroy(server);
    }

    void listen(){
        ENetEvent event;
        while (enet_host_service (server, & event, 1000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    printf ("A new client connected from %x:%u.\n",
                            event.peer -> address.host,
                            event.peer -> address.port);
                    /* Store any relevant client information here. */
                    event.peer -> data = (void *) "Client information";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    printf ("A packet of length %zu containing %s was received from %s on channel %u.\n",
                            event.packet -> dataLength,
                            event.packet -> data,
                            event.peer -> data,
                            event.channelID);
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy (event.packet);

                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf ("%s disconnected.\n", event.peer -> data);
                    /* Reset the peer's client information. */
                    event.peer -> data = NULL;
            }
        }
    }



};
#endif //HALCYONICUS_HALNET_H
