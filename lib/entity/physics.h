//
// Created by Triage on 10/29/2023.
//

#ifndef HALCYONICUS_PHYSICS_H
#define HALCYONICUS_PHYSICS_H

#include <set>
#include "entity.h"
#include "../world/chunk.h"

/**
 * Defines a method for imparting physics calculations onto objects based on a floor, or obstacles.
 * Given a limited scope of information, can completely handle the movement and placement of objects around the world.
 */
class PHYSICS{
 struct energyVector{
    float xImpulse, yImpulse, zImpulse;
    bool isGrounded;
 };

public:

    static float tick(std::vector<CHUNK*>* chunks, ENTITY* entity, energyVector* eVec) {

        COORDINATE::ENTITYCOORD ts[4] = {entity->getLocation(),entity->getLocation(),entity->getLocation(),entity->getLocation()};
        ts[1].manipulate(1,1,0);
        ts[2].manipulate(1,0,0);
        ts[3].manipulate(0,1,0);

        float floor = 0.0;
        char count = 0;

        for (auto ec : ts){

            //info << "EC" << (int)count << ": x" <<ec.x<<" y" << ec.y<<nl;
            //count++;
            for (auto c: *chunks) {
                if (ec.getWorldcoord() != c->location) continue;
                //Derive the point in which we are standing over from this
                int     x = ((int) ec.y) & 0x3f,
                        y = ((int) ec.x) & 0x3f;
                //You dont need this for negative coordinates apparently, its all good. Tried with -1 -1 chunk
                //y = abs(y - 63);
                int cfloor = c->layers[entity->getLayer()].heights[x][y];
                if (cfloor > floor ) floor = cfloor;

            }
        }

        ts[0].z = floor + 2;
        entity->setLocation(ts[0]);
        return floor;
    }
};
#endif //HALCYONICUS_PHYSICS_H
