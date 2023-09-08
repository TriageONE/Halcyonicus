//
// Created by Triage on 8/28/2023.
//

#ifndef HALCYONICUS_CHUNK_H
#define HALCYONICUS_CHUNK_H

#include "../../halcyonicus.h"

class CHUNK{
public:
    class LAYER {
    public:
        // Denotes the level under the ground this layer is where 0
        //  is the topmost layer of the world and 255 is the deepest
        unsigned char level = 0;

        // A list of heights for this layer
        short heights[64][64] {0};

        /* A byte-packed descriptor for materials that exist for each column
         *
         * interpreted as char array[4],
         *  place 0 is the top state
         *  place 1 is the type of column material
         *  place 2-3 is a short prim for the stopping point of hard stone
         */
        int descriptor[64][64] {0};

        LAYER() = default;

        LAYER(unsigned char level) : level{level} {}

    };

    // Its offset denoting location within the region
    short off = 0;

    // Maps denoting the general temperature values from the chunk
    char    temperature[16][16] {0},
            humidity[16][16] {0};

    std::vector<LAYER> layers;

    CHUNK() = default;

    CHUNK(short offset) {
        this->off = std::clamp((int)offset, 0, 4095);
    }

};

#endif //HALCYONICUS_CHUNK_H
