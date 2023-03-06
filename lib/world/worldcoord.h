//
// Created by Aron Mantyla on 2/17/23.
//

#ifndef HALCYONICUS_WORLDCOORD_H
#define HALCYONICUS_WORLDCOORD_H

#include "coordinate.h"
#include "regioncoord.h"

/**
 * A class dedicated to coordinates of specific collections of 64x64 world tiles. This is called a shard, or if you play minecraft, a chunk.
 *
 */
class WORLDCOORD : public COORDINATE {
public:
    WORLDCOORD(int x1, int y1, int z1) : COORDINATE(x1, y1, z1) {
        //The vertical component of the map
        this->x = x1;
        //The horizontal component of the map
        this->y = y1;
        //The height of the map
        this->z = z1;
    }
    WORLDCOORD()=default;

    REGIONCOORD getRegionCoordinates(){
        //The region coordinate is 16 times smaller than a worldcoord, therefore divide by 16, but bit shift instead.
        int x1, y1;
        x1 = this->getX() >> 4;
        y1 = this->getY() >> 4;
        return {x1, y1};
    };
};



#endif //HALCYONICUS_WORLDCOORD_H
