//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>
#include <chrono>
#include <sstream>

ENTITYLOCATION ENTITY::getLocation() {
    return this->location;
}

std::string ENTITY::getType() {
    return this->type;
}


void ENTITY::setLocation(ENTITYLOCATION l) {
    this->location = l;
}

void ENTITY::setType(std::string t) {
    this->type = std::move(t);
}

DYNABLOB *ENTITY::getAttribute(const std::string& attribute) {
    std::map<std::string,DYNABLOB>::iterator it;
    it = attributes.find(attribute);
    if (it == attributes.end()) return nullptr;
    return &attributes.find(attribute)->second;
}

bool ENTITY::getErrored() {
    return errored;
}

void ENTITY::setAttribute(DYNABLOB data, std::string attribute) {
    attributes.erase(attribute);
    attributes.insert( std::pair<std::string, DYNABLOB> (attribute, data) );
}

std::map<std::string, DYNABLOB> ENTITY::getAllAttributes() {
    return this->attributes;
}

string ENTITY::serialize(ENTITY entity){
    // Ignore the type, just worry about extraneous data and location

    ENTITYLOCATION el = entity.getLocation();
    cfloat x(0), y(0), z(0);

    x = el.getX();
    y = el.getY();
    z = el.getZ();

    stringstream ss;

    ss << '{' << x.serialize() << y.serialize() << z.serialize() << '{';

    for (pair<string, DYNABLOB> p : entity.getAllAttributes()){
        ss << '[' << p.first << p.second.serialize() << ']';
    }

    ss << "}}";
    return ss.str();

}

ENTITY ENTITY::deserialize(string entityString){

    if (entityString[0] != '{'){
        cerr << "Attempted to deserialize a malformed entityString, stuck on first char, expected \'{\', got " << entityString[0] << endl;
        return ENTITY();
    }
    if (entityString[19] != '{'){
        cerr << "Attempted to deserialize a malformed entityString, stuck on attribute section initializer, expected \'{\', got " << entityString[19] << endl;
        return ENTITY();
    }

    int len = entityString.length();
    if (entityString[len-1] != '}'){
        cerr << "Attempted to deserialize a malformed entityString, stuck on last char escape, expected \'}\', got " << entityString[len-1] << endl;
        return ENTITY();
    }
    if (entityString[len-2] != '}'){
        cerr << "Attempted to deserialize a malformed entityString, stuck on attribute section escape, expected \'}\', got " << entityString[len-2] << endl;
        return ENTITY();
    }


    ENTITYLOCATION el;
    cfloat  x = cfloat::deserializeToNewCFloat(entityString.substr(1,6)),
            y = cfloat::deserializeToNewCFloat(entityString.substr(7,12)),
            z = cfloat::deserializeToNewCFloat(entityString.substr(13,18));

    el.setX(x);
    el.setY(y);
    el.setZ(z);

    int place = 20; // Start on the first char of the first attribute.
    if (place == len-2) return ENTITY(el);

}

