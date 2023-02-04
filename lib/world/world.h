//
// Created by Triage on 1/31/2023.
//
#include <array>
#include "map.h"
#ifndef HALCYONICUS_WORLD_H
#define HALCYONICUS_WORLD_H

class WORLD {
public:
    /**
     * 0:2, And so a map of veticality was created, consisting of an array, but how large?
     * each tile can have up to 4 states, so 2 bits. Mountain, land, water, ocean.
     * the smallest unit i can work with is 1 byte, therefore 8 bits. 4 Tiles can be stored in each byte.
     * a chunk is derived from a 64x64 area of tiles, or 4096 unique tiles. bar extra compression, this mandates exactly 1024 bytes of data per chunk, for the heightmap
     */
    class HEIGHTMAP : public MAP {
    public:
        /**
         * 0:1, And god said, let there be a way to discern between heights, as the world must know verticality in 4's
         */
        HEIGHTMAP()= default;
    };

    /**
     * 1:1, Thus a new area of the world internal was to be made, a space dedicated to telling climate data about how warm or cold an area was to be.
     * This was to be represented on a scale the same as height, with little variation in the data alone. 4 states require 2 bits, for a total of 4096 areas, requiring 1024 bytes.
     */
    class CLIMATEMAP : public MAP {
    public:
        /**
         * 1:0:1, and with that, god created a way to call each tone of temperature, with no distinct middle ground.
         * There shall be no true center, as perfection in zonal temperate attribution is impossible, even in the world beyond.
         */
        CLIMATEMAP()= default;
    };
    CLIMATEMAP climatemap;
    HEIGHTMAP heightmap;

    WORLD(){
        climatemap = CLIMATEMAP();
        heightmap = HEIGHTMAP();
    }

};



#endif //HALCYONICUS_WORLD_H
