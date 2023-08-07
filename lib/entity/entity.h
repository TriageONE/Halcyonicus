//
// Created by Triage on 2/5/2023.
//

#ifndef HALCYONICUS_ENTITY_H
#define HALCYONICUS_ENTITY_H

#include <string>
#include <utility>
#include <map>
#include "../../deprecated/coordinate.h"
#include "../../deprecated/location.h"
#include "../types/dynablob.h"
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
    ENTITYLOCATION lastSavedLocation;
    /**
    * All entities will be interpreted with a facing
    * If entities do not face in a direction, it is 0. It can never be negative and exceed 360
     * float facing = 0; was the old way of doing this, but i need a 32 bit number to represent 3 angles, X Y and Z
    */


    /**
     * The entity should also have a fully qualified type identifier that follows convention of strings similar to minecrafts way of creating a system of naming objects and things
     * The TYPE is a thing that describes the type of item and this may be redundant or fairly large
     */
    std::string type = "?";

    bool errored = false;
    bool missingType = false;

    std::map<std::string, std::string> attributes;

    //UUID of 0 means the entity is in an error state and cannot be processed
    unsigned long long uuid = 0;

public:

    ENTITY(ENTITYLOCATION location, std::string type, unsigned long long uuid) {
        this->location = location;
        this->type = std::move(type);
        this->uuid = uuid;
    }

    ENTITY(ENTITYLOCATION location, unsigned long long uuid) {
        this->location = location;
        this->uuid = uuid;
    }

    ENTITY() {
        this->errored = true;
        this->missingType = true;
    }

    ENTITY(ENTITYLOCATION location) {
        this->location = location;
        this->missingType = true;
    }

    ENTITY(std::string type) {
        this->location = ENTITYLOCATION(0,0,0);
    }

    //////////////
    //Comparators
    friend bool operator==(const ENTITY& lhs, const ENTITY& rhs) {
        return (lhs.uuid == rhs.uuid);
    }

    friend bool operator>(const ENTITY& lhs, const ENTITY& rhs) {
        return (lhs.uuid > rhs.uuid);
    }

    friend bool operator<(const ENTITY& lhs, const ENTITY& rhs) {
        return (lhs.uuid < rhs.uuid);
    }

    friend bool operator<=(const ENTITY& lhs, const ENTITY& rhs) {
        return (lhs.uuid <= rhs.uuid);
    }

    friend bool operator>=(const ENTITY& lhs, const ENTITY& rhs) {
        return (lhs.uuid >= rhs.uuid);
    }

    friend bool operator!=(const ENTITY& lhs, const ENTITY& rhs) {
        return (lhs.uuid != rhs.uuid);
    }

    //////////////////////////
    // Comparators, extended

    bool operator==(const ENTITY& other) {
        return (this->uuid == other.uuid);
    }

    bool operator>(const ENTITY& other) {
        return (this->uuid > other.uuid);
    }

    bool operator<(const ENTITY& other) {
        return (this->uuid < other.uuid);
    }

    bool operator<=(const ENTITY& other) {
        return (this->uuid <= other.uuid);
    }

    bool operator>=(const ENTITY& other) {
        return (this->uuid >= other.uuid);
    }

    bool operator!=(const ENTITY& other) {
        return (this->uuid != other.uuid);
    }

    ///////////
    //Getters
    ENTITYLOCATION getLocation();
    ENTITYLOCATION getLastSavedLocation();
    std::string getType();
    unsigned long long getUUID();
    float getFacing();
    std::string getAttribute(const std::string& attribute);
    std::map<std::string, std::string> getAllAttributes();
    [[nodiscard]] bool isErrored() const;
    [[nodiscard]] bool isUntyped() const;
    [[nodiscard]] bool hasUUID() const;

    //////////
    //Setters
    void setLocation(ENTITYLOCATION l);
    void setLastSavedLocation(ENTITYLOCATION l);
    void setType(std::string type);
    void setUUID(unsigned long long);
    void setFacing(float);
    void setAttribute(const std::string& dblob, const std::string& attribute);
    void setAttributes(std::map<std::string, std::string> * attributes);

    ////////////
    //Modifiers
    bool removeAttribute(const std::string& attribute);
    bool hasAttribute(const std::string& attribute);
    unsigned long long generateAndSetNewUUID();

    ////////////////
    //Serialization
    std::string serializeEntity();
    static ENTITY deserializeEntity(std::string entityString);

    ////////////
    //Debuggers
    void out();


};
#endif //HALCYONICUS_ENTITY_H
