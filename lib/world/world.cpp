//
// Created by Triage on 1/31/2023.
//

#include "world.h"

std::string WORLD::getSeed() const {
    return seed;
}

void WORLD::generate(){
    this->heightmap.generate();
    this->climatemap.generate();
}
