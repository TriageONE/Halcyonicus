//
// Created by Triage on 4/3/2023.
//

#ifndef HALCYONICUS_ENTITYLOCATION_H
#define HALCYONICUS_ENTITYLOCATION_H

#include "../types/cfloat.h"

class ENTITYLOCATION{

    //Floats will not work due to errors in handling their location
    // we should find the
    cfloat x = 0.0f , y = 0.0f , z = 0.0f;
    /**
     * Creates a location object with a set XY and Z.
     * The location is used for storing data for things that occupy space in the world on a tile-by-tile basis.
     * Things like blocks and machines, aliged to the world grid and unable to be placed halfway inbetween areas should inherit this attribute.
     * @param x The horizontal coordinate, translating LEFT(-) and RIGHT(+), or WEST(-) and EAST(+) within the world
     * @param y The vertical component of the coordinate, translating UP(+) and DOWN(-) or NORTH(+) and SOUTH(-)
     * @param z The depth component of the coordinate, translating UNDERGROUND(-) and ABOVEGROUND(+)
     */
    ENTITYLOCATION(cfloat x, cfloat y, cfloat z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

public:
    ENTITYLOCATION() = default;

    cfloat getX(){
        return this->x;
    }

    cfloat getY(){
        return this->y;
    }

    cfloat getZ(){
        return this->z;
    }

    void setX(cfloat newX){
        this->x = newX;
    }

    void setY(cfloat newY){
        this->y = newY;
    }

    void setZ(cfloat newZ){
        this->z = newZ;
    }

};
#endif //HALCYONICUS_ENTITYLOCATION_H
