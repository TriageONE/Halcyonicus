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
     static void tick(std::vector<CHUNK*>* chunks, ENTITY* entity, energyVector* eVec){
         float h1,h2,h3,h4;

         //Get all the possible chunk coordinates that you may need to pull from
         std::set<COORDINATE::REGIONCOORD> locs{};
         auto temp = entity->getLocation();
         locs.insert(temp.getRegioncoord()); //0,0 offset
         temp.x += 1;
         locs.insert(temp.getRegioncoord()); //1,0 offset
         temp.y += 1;
         locs.insert(temp.getRegioncoord()); //1,1 offset
         temp.x -= 1;
         locs.insert(temp.getRegioncoord()); //0,1 offset

     }
 };
#endif //HALCYONICUS_PHYSICS_H
