//
// Created by Aron Mantyla on 2/9/23.
//

#ifndef HALCYONICUS_LOCATION_H
#define HALCYONICUS_LOCATION_H

#include "coordinate.h"
#include "worldcoord.h"
#include "region.h"
#include "regioncoord.h"

/**
 * A class dedicated to representing actual single tile locations around the world. They have a height, x, and z. Y levels
 * correspond to cave layers and skyheight layers in the top layer of the world.
 * To refer to world shards, that would be the WORLDCOORD class
 * To refer to collections of world shards, that would be the REGIONCOORD class
 *
 * */
class LOCATION : COORDINATE{

public:

    LOCATION(int x, int y, int z) : COORDINATE(x, y, z)  {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    struct RELATIVE{
        int x, z;
        RELATIVE(int x, int z){
            this->x=x;
            this->z=z;
        }
    };

    [[nodiscard]] RELATIVE getRelativeCoordinates() const{
        return {this->x & 63,this->y & 63 };
    }

    //We need a way to get what world shard this is in
    [[nodiscard]] WORLDCOORD getWorldCoord() {
        int x2 = this->x >> 5;
        int y2 = this->y >> 5;
        int z2 = this->z >> 5;
        return {x2,y2,z2};
    }

    //We need a way to get what region this is in
    [[nodiscard]] REGIONCOORD getRegionCoord(){
        int x2 = this->x >> 7;
        int z2 = this->z >> 7;
        return {x2,z2};
    }

};

#endif //HALCYONICUS_LOCATION_H
