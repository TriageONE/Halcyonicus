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

    /**
     * The entity should also have a fully qualified type identifier that follows convention of strings similar to minecrafts way of creating a system of naming objects and things
     * The TYPE is a thing that describes the type of item and this may be redundant or fairly large
     */
    std::string type = "?";

    bool errored = false;
    bool missingType = false;

    std::map<std::string, std::string> attributes;

public:

    ENTITY(ENTITYLOCATION location, std::string type) {
        this->location = location;
        this->type = std::move(type);
    }

    ENTITY() {
        this->errored = true;
        this->missingType = true;
    }

    ENTITY(ENTITYLOCATION location) {
        this->location = location;
        this->missingType = true;
    }
    ///////////
    //Getters
    ENTITYLOCATION getLocation();
    string getType();
    string getAttribute(const string& attribute);
    map<string, string> getAllAttributes();
    [[nodiscard]] bool isErrored() const;
    [[nodiscard]] bool isUntyped() const;


    //////////
    //Setters
    void setLocation(ENTITYLOCATION l);
    void setType(std::string type);
    void setAttribute(const std::string& dblob, const std::string& attribute);
    void setAttributes(map<string, string> * attributes);

    ////////////
    //Modifiers
    bool removeAttribute(const string& attribute);
    bool hasAttribute(const string& attribute);

    ////////////////
    //Serialization
    string serializeEntity();
    static ENTITY deserializeEntity(string entityString);

    ////////////
    //Operators
    friend bool operator==(const ENTITY& lhs, const ENTITY& rhs)
    {
        //Must compare all attributes unless UUID as a attr is used
        if (lhs.type != rhs.type) return false;
        if (lhs.attributes.contains("uuid") && rhs.attributes.contains("uuid")){
            return (lhs.attributes.find("uuid") == rhs.attributes.find("uuid"));
        }
        //If they do not have a uuid then we cannot guarantee that they are equal.
        return false;
    }

    ////////////
    //Debuggers
    void out();


};
#endif //HALCYONICUS_ENTITY_H
