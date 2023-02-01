//
// Created by Triage on 1/31/2023.
//

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
     * a chunk is derived from a 64x64 area of tiles, or 4096 unique tiles. bar compression, this mandates exactly 512 bytes of data per chunk, for the heightmap
     */
    HEIGHT map[512] { OCEAN };
    HEIGHT pick(int place);
    void pack(int place, HEIGHT value);
public:
    void set(HEIGHT h, int x, int y);
    HEIGHT get(int x, int y);

};

#endif //HALCYONICUS_WORLD_H
