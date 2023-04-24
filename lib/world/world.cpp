//
// Created by Triage on 1/31/2023.
//

#include <iostream>
#include <sstream>
#include <utility>
#include "world.h"
#include "../hash/md5.h"

std::string WORLD::getSeed() const {
    return this->seed;
}
void WORLD::constrain() {

    // The scalar of the climate should be zoomed in much more than the heightmap
    this->climatemap.setScalar(heightmap.getScalar() / 6);
    // The world should favor mild climates more than extremes
    this->climatemap.setW0(2);
    this->climatemap.setW1(3);
    this->climatemap.setW2(3);
    this->climatemap.setW3(2);
    // The world should have smooth transitions between climates and mildly warm climates should be slightly more prevalent
    this->climatemap.setRoughness(0);
    this->climatemap.setVBias(0.15);

    // The world should have a saturation map that scales 4 times larger than the normal world map
    this->saturationmap.setScalar(heightmap.getScalar() / 4);
    // The saturation map should favor less extreme amounts of water content
    this->saturationmap.setW0(2);
    this->saturationmap.setW1(3);
    this->saturationmap.setW2(3);
    this->saturationmap.setW3(2);
    // The world should have smooth transitions between saturation and mildly wet climates should be slightly more prevalent
    this->saturationmap.setRoughness(0);
    this->saturationmap.setVBias(0.15);
}

void WORLD::generate(){
    this->heightmap.generate();
    this->climatemap.generate();
    this->saturationmap.generate();
    int i = 0;

    //NOTE: I fixed the generator and found that if i dont put an amprasand before the c in CAVE within the FOR, it creates its own objects and doesnt affect the member instance
    // Im calling this a 'foreaffector' or fO(lre)-fecktor,
    for(CAVE &c : caves){
        if (!c.isInitialized()){
            c.init(this->seed, i, this->worldcoord);
        }
        if (!c.isGenerated()){
            c.generate();
        }
        i++;
    }
    this->generated = true;
}

WORLDCOORD WORLD::getLocation() {
    return this->worldcoord;
}

bool WORLD::isGenerated() const {
    return this->generated;
}

bool WORLD::isInitialized() const {
    return this->initialized;
}

int WORLD::getHash() {
    MD5 md5;

    std::stringstream ss("");
    ss << this->heightmap.getRawHash();
    ss << this->climatemap.getRawHash();
    ss << this->saturationmap.getRawHash();

    for (CAVE c : caves){
        ss << c.getRawHash();
    }

    std::string data = md5(ss.str().c_str(), ss.str().size()).substr(0,3);

    return *(int*) data.c_str();
}

std::string WORLD::getRawHash() {
    std::stringstream ss("");
    ss << this->heightmap.getRawHash();
    ss << this->climatemap.getRawHash();
    ss << this->saturationmap.getRawHash();

    for (CAVE &c : caves){
        ss << c.getRawHash();
    }

    return ss.str();
}

void WORLD::out() {
    using namespace std;

    wcout << "/////////////// Heightmap: ///////////////" << endl;
    cout <<  this->heightmap.getRawHash() << endl;
    this->heightmap.out();
    cout << endl;

    wcout << "/////////////// ClimateMap: ///////////////" << endl;
    cout <<  this->climatemap.getRawHash() << endl;
    this->climatemap.out();
    cout << endl;

    wcout << "/////////////// SaturationMap: ///////////////" << endl;
    cout <<  this->saturationmap.getRawHash() << endl;
    this->saturationmap.out();
    wcout << endl;

    for (CAVE &c : this->caves){
        wcout << "/////////////// C" << c.getLevel() << " ///////////////" << endl;
        cout <<  c.getRawHash() << endl;

        c.out();
        wcout << endl;
    }
}
