//
// Created by Triage on 2/5/2023.
//

#ifndef HALCYONICUS_ENTITY_H
#define HALCYONICUS_ENTITY_H

#include <string>
#include <utility>
#include <map>
#include "../world/coordinate.h"
#include "../world/location.h"
#include "./dynablob.h"
#include "entitylocation.h"

/**
 * 3:2, And so god said the world would then be filled with more than earth, finally branching out and calling itself something more than a boring structure of static life.
 * An entity is anything that is special to the world, anything that is unique to the map that may be interacted with, such as walls, rocks, trees, bushes and grass
 */

class ENTITY {
    /**
    * Philosophicus, 4:0:
    *  To provide nothing more than 2 layers of verticality will severely strip the game of its vertical nature. Imagine a world where you can only travel left and right,
    *  and there is no ground truly below or above you. This would be naturally exhausting, as the amount of 'stuff' you could fit into this game eventually comes to a
    *  plateau, and then only would you be responsible for padding this lack of layered interest with more things to do.
    */

    /**
    * All entities will be interpreted with a location
    * They also must have the region they exist in, and this should be validated against its location
    */
    ENTITYLOCATION location;

    /**
     * The entity should also have a fully qualified type identifier that follows convention of strings similar to minecrafts way of creating a system of naming objects and things
     * The TYPE is a thing that describes the type of item and this may be redundant or fairly large
     */
    std::string type;

    /**
     * The item should have a unique ID to identify itself as something in the world uniquely
     */
    std::string uniqueID;

    std::map<std::string, DYNABLOB> attributes;

public:

    ENTITY(ENTITYLOCATION location, std::string type, std::string uniqueID) {
        this->location = location;
        this->type = std::move(type);
        this->uniqueID = std::move(uniqueID);
    }
    ///////////
    //Getters
    LOCATION getLocation();
    std::string getType();
    std::string getUniqueID();
    DYNABLOB * getAttribute(const std::string& attribute);
    std::map<std::string, DYNABLOB> getAllAttributes();

    //////////
    //Setters
    void setLocation(LOCATION l);
    void setType(std::string type);
    void setUniqueID(std::string uuid);
    void setAttribute(DYNABLOB, std::string attribute);

    ////////////////
    //Serialization
    /**
     * The largest struggle i have is deciding how to serialize entities into a file. We can create entities, modify them,
     * and even destroy them, but the thing i get tripped up about on is how to serialize them.
     *
     * This class isnt actually meant to serialize itself. We should not be worrying about that, since we should have another
     * class responsible for these things. The thing that we need is the ability to look within memory, which is a server
     * side implementation to gather the entities. In fact, the entity serializer should be able to pick at entities as needed
     * because of the way that we grab entities. In memory, we should be able to grab entities by location and other ways
     *
     * Dont serialize entities in this class
     */
};
#endif //HALCYONICUS_ENTITY_H
