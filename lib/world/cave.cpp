//
// Created by Aron Mantyla on 2/6/23.
//

#include "cave.h"
#include <bit>
#include "../hash/md5.h"
#include "../noise/perlin.h"

/**
 * AT LAST, the final sweep of faith, as all that exists shall be completed and seen. The underground shall weave intricate tunnels and spanning structures
 * full of life and story. The underground shall serve as a place to fear and be feared within, and below the ground we shall find great riches and
 * deadly spells, spanning dungeons and watchful eyes with
 */
void CAVE::generate() {
    MD5 md5;
    uint32_t s1 = *(int*) (md5(&seed, 8).c_str());
    s1 = std::rotr(s1, level) + (level * 2);

    const siv::PerlinNoise::seed_type pseed = s1;
    const siv::PerlinNoise perlin{ pseed };
    int xo, yo;
    WORLDCOORD l = getWorldCoord();
    xo = (l.getX() * 64);
    yo = (l.getY() * 64);
    char8_t final = 0;

    level = std::clamp(level, 0, 15);
    float c1,c2;


    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {

            double rnoise = ((
                    perlin.octave2D_01((-1) * (x * 0.5), (-1) * (y * 0.5), 1) * 2) - 1 ) * 0.1;
            if (level == 0) level = 1;

            //As you descent deeper, where 0 is the deepest, you find that the cave expands and is larger than normal
            //At
            float fLevel = staticL[level];
            double noise = perlin.octave2D_01((x + xo) * fLevel, (y + yo) * fLevel, 2) + rnoise;


            //as the cave deepens, water initially should be abundant, with the most on level 14, and the least on level 5, starting on a moderate amount on 15 and increasing again on 0
            //This requires the following function to find out the values for c0:
            // c0 = _cos(_level * (pi()/8) + 1) +2
            // the function is static and only has one variable that never changes so we can implement a LUT to make it faster

            //Naturally as the cave becomes closer to the surface, the cave will be more porous,
            // near the middle of the cave, we may see less cave action, requiring the player to dig more
            // near the bottom of the cave, the cave opens up much more and is abundantly open.
            // c1 = _sin( (i/2 + 7) + _sin( (x/4 + 1) + 4

            //Soft walls should scale inversely with hard walls as we get deeper, but naturally scale up slightly as we get deeper.
            // the solution is a linear scale the scales slightly slower the more you descend
            // The function to represent this is:
            // c2 = -_ln(( _level^ (0.5) * 3 ) + 6 ) + 5;

            //Hard rock should be more abundant the lower you descend, and we should bias this against the upper statement
            // we can use the following to represent the stone abundance:
            // c3 = -_ln(( _level^ (1.2) * 2 ) + 2 ) + 5


            c1 = staticw1[level]+staticw0[level];
            c2 = staticw2[level]+c1;

            // VW2 = w0+w1+w2, therefore vw2+w3 = w0+w1+w2+w3
            double total = noise * ( c2 + staticw3[level] );

            final = 0;

            //Ladder comparator to seek within ranges, could directly cast to char but that defeats the point of weighted averaging
            if(total >= staticw0[0] && total < c1) {
                final = 1;
            } else if (total >= c1 && total < c2){
                final = 2;
            } else if (total >= c2) {
                final = 3;
            }
            set((char8_t) final, x, y);
        }
    }
    generated=true;
}

/**
 * Returns the level of the cave in the ground. Level 0 is the deepest, level 11 is the highest
 * @return The underground level of the cave, from 0 - 11
 */
int CAVE::getLevel() const {
    return this->level;
}

/**
 * Initializes the object such that it has a proper world coordinate, level and seed.
 * @param seed The seed of the world. Should be the same as the overworld
 * @param lvl The deepness of the cave, from 0 - 11
 * @param worldcoord the world coordinate to denote which shard this is from
 */
void CAVE::init(std::string seed, int lvl, WORLDCOORD worldcoord) {
    this->level = lvl;
    this->seed = std::move(seed);
    this->l = worldcoord;
}
