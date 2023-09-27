//
// Created by Triage on 9/19/2023.
//

#ifndef HALCYONICUS_BLOCK_H
#define HALCYONICUS_BLOCK_H

#include <string>
#include <map>
#include <vector>
#include "coordinate.h"
#include "../types/dynablob.h"
#include "../logging/hlogger.h"

/**
 * The Block class is meant to allow for actual physical blocks to be placed within the world
 * each block should have the following characteristics:
 * <ul>
 * <li>A location
 * <li>An orientation, one of 6 directions
 * <li>A type
 * <li>A data section
 * </ul>
 *
 * The location is an integer coordinate that allows for an absolute location within the world, the orientation
 * decides what way its facing, the type defines what the block is, and the data section defines what the block holds
 * in data storage, which could be anything
 *
 * Orientations can be represented with an enum for saftey.
 * type should not be a string. This is important, since strings are useless and redundant here.
 * type should be held with an integer, allowing for about 4 billion blocks. an indexer can take care
 * of translating types to integers
 *
 * attributes are inherent to blocks, and are defined via templates. never should we store data in a block
 *  -  what if we wanted to have an inventory? how would that look?
 *  a. make it so that the template generates a locational reference, where that location will now contain an inventory
 *  -  what about block damage?
 *  a. blocks should be able to hold some sort of damage, but  not in a key value pair. Ideally we would hold this in a dynablob instead which further compresses the class
 *
 * Representing these blocks in data should be separate from the manner of organizing layers. we should expect that each of these objects is present in
 * a vector like object that holds a context for all loaded blocks. the serializer for a block should take into account the type of block and organize based
 * on that.
 *
 * the structure could look like:
 * type {
 *      location, attrs..;
 *      location, attrs..;
 *      location, attrs..;
 * }
 * This way, we would assume that the serialization would already take into account the types here, so then we can focus on the
 * meat, the location and data instead
 * This is pretty dumb though, and overcomplicated since you are performing compression that zlib can already do, so why do it? just put everything together?
 *
 */
using namespace hlogger;
class BLOCK{
public:
    enum ORIENTATION{
        NORTH,
        SOUTH,
        EAST,
        WEST,
        UP,
        DOWN
    } orientation = NORTH;

    unsigned int type = 0;
    COORDINATE location{};
    bool changed = false;

    BLOCK()= default;

    /**
     * Ok, screw this. The complexity scheme of a block SHOULD already be handled by an
     * entity. Placing extra burden on our blocks by just having them store data is useless. have things that can hold data
     * be entities, as they are static and have a place in the world. Marking an entity as to not despawn is fine enough
     *
     * With this scope change, it seems like we can constrain the scope further with location
     * using the idea of offset locality, the absolute position of a block being represented in memory as a really large number, we can
     * effectively give way to describing the actual location of the block through a simpler datatype.
     *
     * if we have a location, we can derive the chunk. Since we store block data in large columns that span an entire chunk at once, we can
     * say that each 64x64 chunk now allows for planes of about 4096 blocks.
     * our max world height is 65535, so an unsigned short. multiply the 2 and we get a number about 16x smaller than an int. this adds to 29 bits needed
     * so we can just use an int to store this with wasted data.
     * @param dst
     */
    void serialize(unsigned char dst[9]){
        int offset = this->location.getChunkOffset();
        ::memcpy(dst, &offset, 4);
        ::memcpy(dst+4, &type, 4);
        dst[8] = (unsigned char) orientation;
    }

    void deserialize(unsigned char src[9]){
        int temp = 0;
        ::memcpy(&temp, src, 4);
        location.setFromOffset(temp);
        ::memcpy(&type, src + 4, 4);
        orientation = (ORIENTATION) src[8];
    }

    void out() const{
        std::string facing;
        switch (orientation) {

            case NORTH:
                facing = "NORTH";
                break;
            case SOUTH:
                facing = "SOUTH";
                break;
            case EAST:
                facing = "EAST";
                break;
            case WEST:
                facing = "WEST";
                break;
            case UP:
                facing = "UP";
                break;
            case DOWN:
                facing = "DOWN";
                break;
        }
        info << "BLOCK OUT: TYPE: " << type << ", LOCATION: " << location.getString() << ", FACING: " << facing << nl;
    }
};


#endif //HALCYONICUS_BLOCK_H
