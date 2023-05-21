//
// Created by Triage on 1/31/2023.
//
#include <array>
#include <vector>
#include "map.h"
#include "../hash/md5.h"
#include "../entity/entity.h"
#include "cave.h"
#include "worldcoord.h"

#ifndef HALCYONICUS_WORLD_H
#define HALCYONICUS_WORLD_H


class WORLD {

protected:

    std::string seed;
    WORLDCOORD worldcoord;
    bool initialized = false, generated = true;
public:
/**
     * 1:1, Thus a new area of the world internal was to be made, a space dedicated to telling climate data about how warm or cold an area was to be.
     * This was to be represented on a scale the same as height, with little variation in the data alone. 4 states require 2 bits, for a total of 4096 areas, requiring 1024 bytes.
     */
    /**
     * 1:0:1, and with that, god created a way to call each tone of temperature, with no distinct middle ground.
     * There shall be no true center, as perfection in zonal temperate attribution is impossible, even in the world beyond.
     */
    /**
    * 1:2, Finally, a way for many maps of all nomenclature to reside under one object, so that when we are handling the art, all things reside in a single place,
    * free from confusion.
    */
    MAP climatemap;

    /**
     * 0:1, And god said, let there be a way to discern between heights, as the world must know verticality in 4's
     */
    /**
     * 0:2, And so a map of veticality was created, consisting of an array, but how large?
     * each tile can have up to 4 states, so 2 bits. Mountain, land, water, ocean.
     * the smallest unit i can work with is 1 byte, therefore 8 bits. 4 Tiles can be stored in each byte.
     * a chunk is derived from a 64x64 area of tiles, or 4096 unique tiles. bar extra compression, this mandates exactly 1024 bytes of data per chunk, for the heightmap
     */
    MAP heightmap;

    /**
     * 3:0, And god said there shall be a way for water and its general measurement to be decided, as the world was full of essence and vaired between place to place
     */
    /**
     * 3:1 As there was no reason to change the amount of resolution required, they kept the data the same, requiring only 4 states of being and not one more
     */
    MAP saturationmap;

    //Entities should not be stored within a world file. Entities saved should be stored in their own files present within a separate directory
    //std::vector<ENTITY> entities {};

    std::array<CAVE, 12> caves {};

    bool operator<(const WORLD& other) const {
        if (worldcoord.getX() < other.worldcoord.getX())
            return true;
        if (worldcoord.getX() > other.worldcoord.getX())
            return false;

        if (worldcoord.getY() < other.worldcoord.getY())
            return true;
        if (worldcoord.getY() > other.worldcoord.getY())
            return false;

        return worldcoord.getZ() < other.worldcoord.getZ();
    }

    WORLD(const std::string& seed, WORLDCOORD w){

        this->seed = seed;

        std::string s1,s2;

        s1 = seed;
        s2 = seed;


        for (int i = 0; i < seed.size(); i++){
            char s1t = s1[i];
            char s2t = s2[i];

            s1t++;
            s2t--;

            s1[i] = s1t;
            s2[i] = s2t;
        }

        heightmap.init(seed,w);
        climatemap.init(s1,w);
        saturationmap.init(s2,w);
        int cn = 0;
        for (CAVE &c : caves){
            c.init(seed, cn, this->worldcoord);
            cn++;
        }
        this->worldcoord = w;
        this->initialized = true;
    }

    /**
     * 1:3, If one wanted to recieve the world's heart, all they have to do is ask and nothing more. The heart may not be much but it is the basis of all creation and light.
     * @return The seed of the world in integer format
     */
    [[nodiscard]] std::string getSeed() const;

    /**
     * 2:0, on the second day, god wanted a way to automate the hard work of creating a meticulously perfect world, and accepted its flaws as a feature.
     * A concept of bundling something called Noise into this world as an intrinsic thing, rather than as an external idea. \\
     *
     * A generator should be made, but there should be a way that it can take opinions and options, things that you may want to have affect the world in a unique way
     * And so a set of functions were built so manipulators of data can be used to affect world generation
     *
     * But at last a realization was made that each of these individual maps were rendered unto their own vices, with each having their own valuation of modifiers.
     * the granularity of setting was initially meant for the entire world, but the world is not that simple and should be subject to any numbers the users throw their way,
     * discriminating on basis of what map rather than *the* map
     */
    void generate();
    void constrain();

    //////////////
    //Getters

    WORLDCOORD getLocation();
    [[nodiscard]] bool isInitialized() const;
    [[nodiscard]] bool isGenerated() const;

    int getHash();

    std::string getRawHash();
    void out();

};

#endif //HALCYONICUS_WORLD_H
