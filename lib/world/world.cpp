//
// Created by Triage on 1/31/2023.
//

#include "world.h"

std::string WORLD::getSeed() const {
    return seed;
}

void WORLD::constrain() const{

    //Build quick aliases for setting values instead of typing 'this->[map].?'
    MAP h = this->heightmap;
    MAP s = this->saturationmap;
    MAP c = this->climatemap;

    // The scalar of the climate should be zoomed in much more than the heightmap
    c.set_scalar(h.get_scalar()/6);
    // The world should favor mild climates more than extremes
    c.set_w0(2);
    c.set_w1(3);
    c.set_w2(3);
    c.set_w3(2);
    // The world should have smooth transitions between climates and mildly warm climates should be slightly more prevalent
    c.set_roughness(0);
    c.set_vBias(0.15);

    // The world should have a saturation map that scales 4 times larger than the normal world map
    s.set_scalar(h.get_scalar()/4);
    // The saturation map should favor less extreme amounts of water content
    s.set_w0(2);
    s.set_w1(3);
    s.set_w2(3);
    s.set_w3(2);
    // The world should have smooth transitions between saturation and mildly wet climates should be slightly more prevalent
    s.set_roughness(0);
    s.set_vBias(0.15);
}

void WORLD::generate(){
    this->heightmap.generate();
    this->climatemap.generate();
    this->saturationmap.generate();
}
