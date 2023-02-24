//
// Created by Aron Mantyla on 2/17/23.
//

#ifndef HALCYONICUS_COORDINATE_H
#define HALCYONICUS_COORDINATE_H

/**
 * A class dedicated to representing any ambiguous coordinate that may be relative to another coordinate. This is a simple class that contains an X Y and Z
 */
class COORDINATE {
protected:
    int x{};
    int y{};
    int z{};
public:

    COORDINATE(int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    COORDINATE() {}


    void setX(int intX){ this->x = intX;}
    void setY(int intY){ this->y = intY;}
    void setZ(int intZ){ this->z = intZ;}

    void set(int intX, int intY, int intZ){
        this->x = intX;
        this->y = intY;
        this->z = intZ;
    }

    [[nodiscard]] int getY() const{return this->y;}
    [[nodiscard]] int getZ() const{return this->z;}
    [[nodiscard]] int getX() const{return this->x;}
};
#endif //HALCYONICUS_COORDINATE_H
