//
// Created by Triage on 1/31/2023.
//

#include <iostream>
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
    LOCATION tLoc = location;
    int i = 0;
    for(CAVE c : caves){
        std::printf("CAVE %i:\n", i);
        tLoc.setY(i);
        if (!c.isInitialized()){
            c.init(seed, i);
        }
        if (!c.isGenerated()){
            c.generate();
        }
        c.out();
        std::cout << std::endl;
        i++;
    }
    generated = true;
}

std::array<CAVE, 16>* WORLD::getCaves() {
    return &caves;
}

LOCATION WORLD::getLocation() {
    return this->location;
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

std::vector<ENTITY>* WORLD::getEntities() {
    return &entities;
}
