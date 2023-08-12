//
// Created by Aron Mantyla on 2/17/23.
//

#ifndef HALCYONICUS_WORLDCOORD_H
#define HALCYONICUS_WORLDCOORD_H

#include "regioncoord.h"

/**
 * A class dedicated to coordinates of specific collections of 16x16 world tiles. This is called a shard, or if you play minecraft, a chunk.
 *
 */
class WORLDCOORD{
    int x=0, y=0;
public:
    WORLDCOORD(int x1, int y1) {
        //The vertical component of the map
        this->x = x1;
        //The height of the map
        this->y = y1;
    }
    WORLDCOORD()=default;

    COORDINATE getRegionCoordinates(){
        //The region coordinate is 16 times smaller than a worldcoord, therefore divide by 16, but bit shift instead.
        int x1, y1;
        x1 = this->x >> 4;
        y1 = this->y >> 4;
        return {x1, y1};
    };

    int getX(){
        return this->x;
    }

    int getZ(){
        return this->y;
    }

    void setX(int newX){
        this->x = newX;
    }

    void setZ(int newZ){
        this->y = newZ;
    }

    bool operator<(const WORLDCOORD& other) const {
        if (this->x < other.x)
            return true;
        if (this->x > other.x)
            return false;

        return this->y < other.y;
    }

    bool operator==(const WORLDCOORD& other) const{
        return (
                other.x == this->x &&
                other.y == this->y
                );
    }

    bool operator!=(const WORLDCOORD& other) const{
        return (
                other.x != this->x &&
                other.y != this->y
        );
    }
};



#endif //HALCYONICUS_WORLDCOORD_H
