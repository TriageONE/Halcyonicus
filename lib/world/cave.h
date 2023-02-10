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
    /**
     * 5:0 after leading those to the underground, the lord said there shall be more than just depth, and though untested and unseen his result is final
     */
    constexpr static float staticw0[16]{2.54031, 2.82119, 2.97706, 2.98418, 2.84147, 2.57065, 2.21296, 1.82284, 1.4597, 1.17881, 1.02294, 1.01582, 1.15853, 1.42935, 1.78704, 2.17716,};
    constexpr static float staticw1[16]{3.69136, 4.2338, 4.2338, 3.66336, 3.66336, 3.60455, 3.60455, 3.14113, 2.54113, 2.23228, 1.56528, 2.32142, 3.32142, 4.83083, 5.83083, 6.59846,};
    constexpr static float staticw2[16]{2.10963, 2.13103, 2.15364, 2.17763, 2.20319, 2.23055, 2.26001, 2.29195, 2.32687, 2.36543, 2.4086, 2.45775, 2.51509, 2.58443, 2.67344, 1.20278,};
    constexpr static float staticw3[16]{1.89477, 1.98229, 2.07557, 2.1754, 2.28276, 2.39886, 2.52523, 2.66382, 2.81718, 2.98873, 3.1832, 3.4073, 3.67098, 3.9895, 4.38639, 4.89056,};
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
