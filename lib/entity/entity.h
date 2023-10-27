//
// Created by Triage on 2/5/2023.
//

#ifndef HALCYONICUS_ENTITY_H
#define HALCYONICUS_ENTITY_H

#include <map>
#include <random>
#include "../world/coordinate.h"
#include "../types/dynablob.h"
#include "../tools/timetools.h"
#include "../../graphics/Shader.h"
#include "../../graphics/Model.h"
#include "glm/gtx/quaternion.hpp"


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
    COORDINATE::ENTITYCOORD location;
    COORDINATE::ENTITYCOORD lastSavedLocation;
    unsigned long long timeLastSaved;
    float facing;
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

    ENTITY(COORDINATE::ENTITYCOORD location, std::string type, unsigned long long uuid) {
        this->location = location;
        this->type = std::move(type);
        this->facing = 0;
        this->uuid = uuid;
    }

    ENTITY(COORDINATE::ENTITYCOORD location, std::string type) {
        this->location = location;
        this->facing = 0;
        this->type = std::move(type);
        generateAndSetNewUUID();
    }

    ENTITY() {
        this->errored = true;
        this->missingType = true;
        generateAndSetNewUUID();
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

    bool operator==(const ENTITY& other) const {
        return (this->uuid == other.uuid);
    }

    bool operator>(const ENTITY& other) const {
        return (this->uuid > other.uuid);
    }

    bool operator<(const ENTITY& other) const {
        return (this->uuid < other.uuid);
    }

    bool operator<=(const ENTITY& other) const {
        return (this->uuid <= other.uuid);
    }

    bool operator>=(const ENTITY& other) const {
        return (this->uuid >= other.uuid);
    }

    bool operator!=(const ENTITY& other) const {
        return (this->uuid != other.uuid);
    }

    ///////////
    //Getters
    COORDINATE::ENTITYCOORD getLocation();
    COORDINATE::ENTITYCOORD getLastSavedLocation();
    std::string getType();
    unsigned long long getUUID() const;
    float getFacing() const;
    std::string getAttribute(const std::string& attribute);
    std::map<std::string, std::string> getAllAttributes();
    [[nodiscard]] bool isErrored() const;
    [[nodiscard]] bool isUntyped() const;
    [[nodiscard]] bool hasUUID() const;

    //////////
    //Setters
    void setLocation(COORDINATE::ENTITYCOORD l);
    void setLastSavedLocation(COORDINATE::ENTITYCOORD l);
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
    void updateTimestamp();
    void updateTimestamp(long long time);

    ////////////////
    //Serialization
    void serializeEntity(std::vector<char> * serialOut);
    void decodeEntityData(std::string entityString);

    ////////////
    //Debuggers
    void out();

    ////////////////
    //Display
    void draw(Shader *shader, Model* model);


};
#endif //HALCYONICUS_ENTITY_H
