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
    constexpr static float staticL[16]{0.0155245, 0.083102, 0.131049, 0.16824, 0.198627, 0.224318, 0.246574, 0.266204, 0.283764, 0.299649, 0.314151, 0.327492, 0.339843, 0.351342, 0.362098, 0.372202};
    /**
     * 5:0 after leading those to the underground, the lord said there shall be more than just depth, and though untested and unseen his result is final
     */
    constexpr static int staticw0[16]{25, 28, 29, 29, 28, 25, 22, 18, 14, 17, 10, 10, 11, 14, 17, 21};
    constexpr static int staticw1[16]{36, 42, 42, 36, 36, 36, 36, 31, 25, 22, 15, 23, 33, 48, 58, 65};
    constexpr static int staticw2[16]{21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 24, 24, 25, 25, 26, 12};
    constexpr static int staticw3[16]{18, 19, 20, 21, 22, 23, 25, 26, 28, 29, 31, 34, 36, 39, 43, 48};
public:
    CAVE(std::string seed, int level){
        this->setSeed(std::move(seed));
        this->level = level;
        initialized = true;
    };
    CAVE()= default;

    [[nodiscard]] int getLevel() const;
    void generate() override;
    void init(std::string seed, int l);
};

#endif //HALCYONICUS_CAVE_H
