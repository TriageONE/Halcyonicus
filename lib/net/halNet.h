//
// Created by Triage on 4/20/2023.
//

#ifndef HALCYONICUS_HALNET_H
#define HALCYONICUS_HALNET_H

/**
 * HALNET is the backbone networking solution to everything halcyonicus.
 * Its an abstraction on top of UDP and works to patch all systems together.
 */
class HALNET{

    /*
     *  Packet header construction should go as follows:
     *  Typical UDP headers present within packet,
     *  DST PORT 1982 for Player control
     *  DST PORT 1999 for Server control
     *
     *  In order to connect to a server, you must have proper authentication, which comes from HSV Blockchain, however this may be implemented down the line
     *  You must also utilize and support some sort of encryption for socket layer and transport layer security, AES512
     *
     *
     */

};
#endif //HALCYONICUS_HALNET_H
