//
// Created by Triage on 1/31/2023.
//

#include <iostream>
#include <sstream>
#include "world.h"
#include "../hash/md5.h"

std::string WORLD::getSeed() const {
    return seed;
}
void WORLD::constrain() {

    // The scalar of the climate should be zoomed in much more than the heightmap
    climatemap.setScalar(heightmap.getScalar() / 6);
    // The world should favor mild climates more than extremes
    climatemap.setW0(2);
    climatemap.setW1(3);
    climatemap.setW2(3);
    climatemap.setW3(2);
    // The world should have smooth transitions between climates and mildly warm climates should be slightly more prevalent
    climatemap.setRoughness(0);
    climatemap.setVBias(0.15);

    // The world should have a saturation map that scales 4 times larger than the normal world map
    saturationmap.setScalar(heightmap.getScalar() / 4);
    // The saturation map should favor less extreme amounts of water content
    saturationmap.setW0(2);
    saturationmap.setW1(3);
    saturationmap.setW2(3);
    saturationmap.setW3(2);
    // The world should have smooth transitions between saturation and mildly wet climates should be slightly more prevalent
    saturationmap.setRoughness(0);
    saturationmap.setVBias(0.15);
}

void WORLD::generate(){
    heightmap.generate();
    climatemap.generate();
    saturationmap.generate();
    int i = 0;
    for(CAVE &c : caves){
        std::printf("CAVE %i:\n", i);
        if (!c.isInitialized()){
            c.init(seed, i, worldcoord);
        }
        if (!c.isGenerated()){
            c.generate();
        }
        i++;
    }
    generated = true;
}

std::array<CAVE, 12>* WORLD::getCaves() {
    return &caves;
}

WORLDCOORD WORLD::getLocation() {
    return this->worldcoord;
}

bool WORLD::isGenerated() const {
    return generated;
}

bool WORLD::isInitialized() const {
    return initialized;
}

MAP* WORLD::getClimatemap() {
    return &climatemap;
}

MAP* WORLD::getHeightmap() {
    return &heightmap;
}

MAP* WORLD::getSaturationmap() {
    return &saturationmap;
}

int WORLD::getHash() {
    std::stringstream ss("");
    ss << heightmap.getRawHash();
    ss << climatemap.getRawHash();
    ss << saturationmap.getRawHash();

    std::array<CAVE, 12>* pArray = getCaves();
    for (CAVE c : *pArray){
        ss << c.getRawHash();
    }

    MD5 md5;
    std::string str = md5(ss.str());
    int out;
    char topHash[4];

    for (int i = 0; i < 4; i++ ){
        topHash[i] = str[i];
    }
    ::memcpy(&out, &topHash, 4);

    return out;
}

std::string WORLD::getRawHash() {
    std::stringstream ss("");
    ss << heightmap.getRawHash();
    ss << climatemap.getRawHash();
    ss << saturationmap.getRawHash();

    std::array<CAVE, 12>* pArray = getCaves();
    for (CAVE c : *pArray){
        ss << c.getRawHash();
    }

    MD5 md5;
    std::string str = md5(ss.str());

    return str;
}
