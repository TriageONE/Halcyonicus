//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>
#include <sstream>

LOCATION ENTITY::getLocation() {
    return this->location;
}

std::string ENTITY::getType() {
    return this->type;
}

std::string ENTITY::getUniqueID() {
    return this->uniqueID;
}


void ENTITY::setLocation(LOCATION l) {
    this->location = l;
}

void ENTITY::setType(std::string t) {
    this->type = std::move(t);
}

void ENTITY::setUniqueID(std::string uuid) {
    this->uniqueID = std::move(uuid);
}

DYNABLOB *ENTITY::getAttribute(const std::string& attribute) {
    std::map<std::string,DYNABLOB>::iterator it;
    it = attributes.find(attribute);
    if (it == attributes.end()) return nullptr;
    return &attributes.find(attribute)->second;
}

void ENTITY::setAttribute(DYNABLOB data, std::string attribute) {
    attributes.erase(attribute);
    attributes.insert( std::pair<std::string, DYNABLOB> (attribute, data) );
}

std::map<std::string, DYNABLOB> ENTITY::getAllAttributes() {
    return this->attributes;
}



