//
// Created by Aron Mantyla on 2/9/23.
//

#ifndef HALCYONICUS_LOCATION_H
#define HALCYONICUS_LOCATION_H
/**
 * A class dedicated to representing locations across the world, with translations to other locations present to ease finding relative coords
 * and other important bits of data. This is to be used for deciding where certain world tiles exist
 */
class LOCATION{

    //Canon coordinates are explicity zero based coordinates that respect the actuality of the real world
    int canonX{}, canonY{}, canonZ{};
    bool initialized = false;

public:

    LOCATION()=default;

    [[maybe_unused]] LOCATION(int x, int y, int z) {
        this->canonX = x;
        this->canonY = y;
        this->canonZ = z;
        this->initialized = true;
    }

    //Getters
    [[nodiscard]] int getX() const { return canonX; }
    [[nodiscard]] int getY() const { return canonY; }
    [[nodiscard]] int getZ() const { return canonZ; }

    //Askers
    [[nodiscard]] bool isInitialized() const { return initialized; }

    //Setters
    void setX(int x){ this->canonX = x; }
    void setY(int y){ this->canonY = y; }
    void setZ(int z){ this->canonZ = z; }

    void set(int x, int y, int z){
        this->canonX = x;
        this->canonY = y;
        this->canonZ = z;
        this->initialized=true;
    }

    struct RELATIVE{
        int x, z;
        RELATIVE(int x, int z){
            this->x=x;
            this->z=z;
        }
    };

    //Transmogrifiers
    [[nodiscard]] RELATIVE getRelativeCoordinates() const{
        return {this->canonX % 64,this->canonY % 64 };
    }


};

#endif //HALCYONICUS_LOCATION_H
