//
// Created by Triage on 1/31/2023.
//
#import "array"
#ifndef HALCYONICUS_WORLD_H
#define HALCYONICUS_WORLD_H

/**
 * And god said, let there be a way to discern between heights, as the world must know verticality in 4s
 */
enum HEIGHT{
    OCEAN,
    WATER,
    LAND,
    MOUNTAIN
};
/**
* In the beginning, god said, let there be a platform, nothing more. All is up for interpretation, intterpolation, and subjection;
* And so he created a class of objects to hold the universe, all in data, no matter how large or small.
 * It shall describe the universe as we see it, and contain secrets unknown.
*/

class HEIGHTMAP {
private:
    /**
     * And so a heightmap was created, consisting of an array, but how large?
     * each tile can have up to 4 states, so 2 bits. Mountain, land, water, ocean.
     * the smallest unit i can work with is 1 byte, therefore 8 bits. 4 Tiles can be stored in each byte.
     * a chunk is derived from a 64x64 area of tiles, or 4096 unique tiles. bar extra compression, this mandates exactly 1024 bytes of data per chunk, for the heightmap
     */
    char8_t map[1024] { 0 };

public:
    //God Says: "Let there be a way for one to manipulate individual places on the master array, treating the array as a large array rather than a coordinate plane
    HEIGHT pick(int place);
    void pack(int place, HEIGHT value);

    //God says: "Let there be a way for those such as yourself and your consort to work upon the map, as if it were a coordinate plane read like a book. We shall find the correct placement for you"
    void set(HEIGHT h, int x, int y);
    HEIGHT get(int x, int y);

    //God says: "Let there be a way for those to see, one way in memory, and another in a monofaregraph, so that we may understand the root of our product"
    void dump_map();
    void out();

    //God says: "And so we shall let the user poke and prod, as they know what is best and why- with only their product to show, the power is theirs to have"
    std::array<unsigned char, 1024> copy_map();
    void set_map(std::array<unsigned char, 1024> new_map);

    //God says: "Let there be a way for those who maniplate the map at a granular or programmable level to easily interface, as reinventing the wheel is pointless and wastes your time"
    static unsigned char compress(std::array<HEIGHT, 4>);
    static std::array<HEIGHT, 4> uncompress(unsigned char c);
};

#endif //HALCYONICUS_WORLD_H
