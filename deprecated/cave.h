//
// Created by Aron Mantyla on 2/6/23.
//

#ifndef HALCYONICUS_CAVE_H
#define HALCYONICUS_CAVE_H

#include <utility>
#include "map.h"

/**
 * A special level class that can contain its own vector of entities, and a simpler world map with perlin noise as a map of areas meant to be dug and filled with dirt
 */
class CAVE : public MAP {
    int level = 0;
    constexpr static float staticL[12]{ 0.071, 0.072, 0.076, 0.078, 0.080, 0.0829649, 0.0914151, 0.0927492, 0.0959843, 0.0961342, 0.0972098, 0.102202};
    /**
     * 5:0 after leading those to the underground, the lord said there shall be more than just depth, and though untested and unseen his result is final
     */
    constexpr static int staticw0[12]{28, 25, 22, 18, 14, 17, 10, 10, 11, 14, 17, 21};
    constexpr static int staticw1[12]{36, 36, 36, 31, 25, 22, 15, 23, 33, 48, 58, 65};
    constexpr static int staticw2[12]{22, 22, 22, 22, 23, 23, 24, 24, 25, 25, 26, 41};
    constexpr static int staticw3[12]{22, 23, 25, 26, 28, 29, 31, 34, 36, 39, 43, 57};
public:

    /**
     * Creates a new cave with a seed and a level
     * @param seed the seed of the overworld
     * @param level The depth of the cave
     */
    CAVE(std::string seed, int level){
        this->setSeed(std::move(seed));
        this->level = level;
        initialized = true;
    };

    CAVE()= default;

    /**
     * Returns the level of the cave in the ground. Level 0 is the deepest, level 11 is the highest
     * @return The underground level of the cave, from 0 - 11
     */
    [[nodiscard]] int getLevel() const;

    /**
     * AT LAST, the final sweep of faith, as all that exists shall be completed and seen. The underground shall weave intricate tunnels and spanning structures
     * full of life and story. The underground shall serve as a place to fear and be feared within, and below the ground we shall find great riches and
     * deadly spells, spanning dungeons and watchful eyes with
     */
    void generate() override;

    /**
     * Initializes the object such that it has a proper world coordinate, level and seed.
     * @param seed The seed of the world. Should be the same as the overworld
     * @param lvl The deepness of the cave, from 0 - 11
     * @param worldcoord the world coordinate to denote which shard this is from
     */
    void init(std::string seed, int l, WORLDCOORD worldcoord);
};

#endif //HALCYONICUS_CAVE_H
