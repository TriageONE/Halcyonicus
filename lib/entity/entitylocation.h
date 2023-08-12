//
// Created by Triage on 4/3/2023.
//

#ifndef HALCYONICUS_ENTITYLOCATION_H
#define HALCYONICUS_ENTITYLOCATION_H

class ENTITYLOCATION{

    //Floats will not work due to errors in handling their location
    /*
     * Z value, height, should not be a cFloat and instead can be a char since the earth is flat, but has layers like an onion
     */
public:
    float x = 0.0f ,y = 0.0f, facing = 0.0f;
    char z = 0.0f;


    ENTITYLOCATION() = default;

    float getX(){
        return this->x;
    }

    float getY(){
        return this->y;
    }

    char getZ(){
        return this->z;
    }

    char getFacing(){
        return this->facing;
    }

    void setX(float newX){
        this->x = newX;
    }

    void setY(float newY){
        this->y = newY;
    }

    void setZ(char newZ){
        this->z = newZ;
    }

    void setFacing(char newFacing){
        this->facing = newFacing;
    }

    /**
     * Creates a location object with a set XY and Z.
     * The location is used for storing data for things that occupy space in the world on a tile-by-tile basis.
     * Things like blocks and machines, aliged to the world grid and unable to be placed halfway inbetween areas should inherit this attribute.
     * @param x The horizontal coordinate, translating LEFT(-) and RIGHT(+), or WEST(-) and EAST(+) within the world
     * @param y The vertical component of the coordinate, translating UP(+) and DOWN(-) or NORTH(+) and SOUTH(-)
     * @param z The depth component of the coordinate, translating UNDERGROUND(-) and ABOVEGROUND(+)
     */
    ENTITYLOCATION(float x, float y, char z, float facing) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->facing = facing;
    }

};
#endif //HALCYONICUS_ENTITYLOCATION_H
