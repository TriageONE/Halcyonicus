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

/**
 * 3:2, And so god said the world would then be filled with more than earth, finally branching out and calling itself something more than a boring structure of static life.
 * An entity is anything that is special to the world, anything that is unique to the map that may be interacted with, such as walls, rocks, trees, bushes and grass
 */

class ENTITY {

    /**
    * An entity must have a location in order to exist
    */

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
    LOCATION location;

    /**
    * Not all things should have health. Something with no health has infinite life.
    */
    //unsigned int maxHealth = 1;
    //unsigned int currentHealth = 1;

    /**
     * All things must be resistant in some way, and there are a lot of resistances.
     * Resistance curves are defined by the master deductor, a function designed to take a resistance and spit out a percentage.
     * This is the amount of damage that should be deducted from the total amount of damage recieved.
     * <br>
     * The deductor should be a function that derives the damage recieved from an attack considering the individual resistances present 
     */

    /**
     * This is an intrinsic value that should be tested for within JSON data as a singlet field found in the tags area. Within JSON data, we should be able to define tags
     * for entities where each tag describes something about the entity such as INV means invulnerable, INVIS means Invisible, SHK means Shocked, etc.
     * */
    //bool invincible = false;

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

    ENTITY(LOCATION location, std::string type, std::string uniqueID) {
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
     * PCAS stands for Patternized Casted Attribute String
     * Patternized, because it follows a convention from the next;
     * Casted, like in real life, a mold has specific ruts, grooves, shapes and ways of forming the metal, or in this case data. It comes as a .hcas file and a follows the YAML convention
     * Attribute, for the things it describes the attributes by name for the processor and ticker to work on and make decisions for
     *
     * This function serves as a serializer method for compressing this data as much as possible for later usage or storage. We can use this when constructing packets for players or saving data to disk
     * @return The stringified version of this class
     */
    std::string getPCAS();
};
#endif //HALCYONICUS_ENTITY_H
//(Input / Codecs > Audio codecs > FluidSynth).