//
// Created by Triage on 2/5/2023.
//

#ifndef HALCYONICUS_ENTITY_H
#define HALCYONICUS_ENTITY_H

#include <string>
#include "../json/json.h"
#include "../world/coordinate.h"
#include "../world/location.h"

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
    * All entities will be interpreted with a location as an integer, however the minimum resolution for movement will be a tenth of a block, to ensure smooth movement.
    */

    LOCATION location;

    /**
    * All things must have some sort of health, and by default they always have at least one health.
    */
    int maxHealth = 1;
    int currentHealth = 1;

    /**
     * All things must be resistant in some way, and there are a lot of resistances.
     * Resistance curves are defined by the master deductor, a function designed to take a resistance and spit out a percentage.
     * This is the amount of damage that should be deducted from the total amount of damage recieved.
     * <br>
     * The deductor should be a function that derives the damage recieved from an attack considering the individual resistances present 
     */

    /**
    * If something is to be marked as invulnerable, it cannot take damage from any source but can have its health set to specific values. by default, this is false.
    */
    bool invincible = false;

    /**
     * The item should have a unique ID and a real name, so that each thing may be identifiable in its rawest form.
     * A uniqueID would be the fully qualified name of the object or entity
     * A real name is the actual name, visible to the user and human readable
     */
    std::string type;
    std::string uniqueID;

    /**
     * All things must have a jsonData of sorts so they may be identified and have other properties
     */
    nlohmann::json jsonData;


public:
    ///////////
    //Getters

    LOCATION getLocation();
    [[nodiscard]] int getMaxHealth() const;
    [[nodiscard]] int getCurrentHealth() const;
    [[nodiscard]] bool isInvincible() const;
    nlohmann::json getData();

    //////////
    //Setters
    void setLocation(LOCATION l);
    void setMaxHealth(int maxHealth);
    void setCurrentHealth(int currentHealth);
    void setInvincibility(bool invincible);
    void setData(nlohmann::json json);

    //////////////
    //Translators
    void decrementHealth();
    void incrementHealth();
    void changeHealthBy(int hp);

    ////////////////
    //Serialization
    /**
     * NCAT stands for Patternized Compressed Attribute String
     * This function serves as a serializer method for compressing this data as much as possible for later usage or storage. We can use this when constructing packets for players or saving data to disk
     * @return The stringified version of this class
     */
    std::string compileToPCAS();
};
#endif //HALCYONICUS_ENTITY_H
