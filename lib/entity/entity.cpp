//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>
#include <sstream>

LOCATION ENTITY::getLocation() {
    return this->location;
}

void ENTITY::setLocation(LOCATION l) {
    this->location = l;
}

//TODO: GETER DONE.. SOON
std::string ENTITY::compileToPCAS() {
    std::stringstream pcas;

    /**
     * Base entity format
     */
    return std::string();
}




