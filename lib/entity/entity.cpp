//
// Created by Triage on 2/5/2023.
//
#include "entity.h"
#include <utility>
#include <sstream>

LOCATION ENTITY::getLocation() {
    return this->location;
}

nlohmann::json ENTITY::getData() {
    return this->jsonData;
}

void ENTITY::setLocation(LOCATION l) {
    this->location = l;
    this->region = l.getRegionCoord();
}

REGIONCOORD ENTITY::getRegion(){
    return this->region;
}


void ENTITY::setData(nlohmann::json json) {
    this->jsonData = std::move(json);
}


//TODO: GETER DONE.. SOON
std::string ENTITY::compileToPCAS() {
    std::stringstream pcas;

    /**
     * Base entity format
     */
    return std::string();
}




