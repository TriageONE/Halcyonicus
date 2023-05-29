//
// Created by Aron Mantyla on 2/9/23.
//

#ifndef HALCYONICUS_LOCATION_H
#define HALCYONICUS_LOCATION_H

#include "coordinate.h"
#include "../lib/world/worldcoord.h"
#include "../lib/world/region.h"
#include "../lib/world/regioncoord.h"

/**
 * A class dedicated to representing actual single tile locations around the world. They have a height, x, and z. Y levels
 * correspond to cave layers and skyheight layers in the top layer of the world.
 * To refer to world shards, that would be the WORLDCOORD class
 * To refer to collections of world shards, that would be the REGIONCOORD class
 * */
class LOCATION : COORDINATE{

public:

    /**
     * Creates a location object with a set XY and Z.
     * The location is used for storing data for things that occupy space in the world on a tile-by-tile basis.
     * Things like blocks and machines, aliged to the world grid and unable to be placed halfway inbetween areas should inherit this attribute.
     * @param x The horizontal coordinate, translating LEFT(-) and RIGHT(+), or WEST(-) and EAST(+) within the world
     * @param y The vertical component of the coordinate, translating UP(+) and DOWN(-) or NORTH(+) and SOUTH(-)
     * @param z The depth component of the coordinate, translating UNDERGROUND(-) and ABOVEGROUND(+)
     */
    [[deprecated]] LOCATION(int x, int y, int z) : COORDINATE(x, y, z)  {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    [[deprecated]] LOCATION() = default;

    /**
     * A relative coordinate that specifies itself as having a value strictly in-between 0 and 63, denoting the place within the current world shard this object exists at.
     */
    struct [[deprecated]] RELATIVE{
        int x, z;
        RELATIVE(int x, int z){
            this->x=x;
            this->z=z;
        }
    };

    /**
     * Gets the relative coordinates of the location, in accordance to how the grid of tiles is set up
     * @return
     */
    [[nodiscard]] RELATIVE getRelativeCoordinates() const{
        return {this->x & 63,this->y & 63 };
    }

    //We need a way to get what world shard this is in
    /**
     * Provides a way to determine what chunk a specific location is in
     * @return The worldcoord this exists in
     */
    [[nodiscard]] WORLDCOORD getWorldCoord() {
        int x2 = this->x >> 5;
        int y2 = this->y >> 5;
        int z2 = this->z >> 5;
        return {x2,y2,z2};
    }

    //We need a way to get what region this is in
    /**
     * Provides a way to determine what region a specific location is in
     * @return The region this exists in
     */
    [[nodiscard]] REGIONCOORD getRegionCoord(){
        int x2 = this->x >> 7;
        int z2 = this->z >> 7;
        return {x2,z2};
    }

};

#endif //HALCYONICUS_LOCATION_H
