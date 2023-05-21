//
// Created by Triage on 5/20/2023.
//

#ifndef HALCYONICUS_HALSERVER_H
#define HALCYONICUS_HALSERVER_H

#include "../lib/context/hcontext.h"
/**
 * HALSERVER allows us to run a context we can pull from and use, and should be refrenced from externally and internally. A flip of the switch and you now can accept incoming connections
 *
 * For the player launching this, they should be able to attach to it and read as needed.
 * A context is used to track all available and loaded zones,
 *
 * When a player joins singleplayer, they will create a server and a context for the server.
 *  Should they create another context for themselves? probably not.
 *
 * When a player joins a multiplayer session, they create a context for themselves and attaches to the network server via halnet
 */
class HALSERVER{
    bool online = false;
    HCONTEXT currentContext;
    std::set<ENTITY> players {};


};
#endif //HALCYONICUS_HALSERVER_H
