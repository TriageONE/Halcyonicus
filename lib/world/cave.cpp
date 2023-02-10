//
// Created by Aron Mantyla on 2/6/23.
//

#include "cave.h"
#include <bit>
#include <utility>
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
    s1 = std::rotr(s1, level) + level * 2;

    const siv::PerlinNoise::seed_type pseed = s1;
    const siv::PerlinNoise perlin{ pseed };
    int xo, yo;
    LOCATION l = getLocation();
    xo = (l.getX() * 64);
    yo = (l.getY() * 64);

    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {

            double rnoise = ((
                    perlin.octave2D_01((-1) * (x * 0.5), (-1) * (y * 0.5), 1) * 2) - 1 ) * 0.2;

            float levelscale = 0.1f / ((float)level/3);

            const double noise = perlin.octave2D_01((x + xo) * levelscale, (y + yo) * levelscale, 2) + rnoise;

            float c0,c1,c2,c3;
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

            char8_t final = 0;

            level = std::clamp(level, 0, 15);

            c0=staticw0[level];
            c1=staticw1[level] + c0;
            c2=staticw2[level] + c1;
            c3=staticw3[level] + c2;

            double total = noise * ( c0+c1+c2+c3 );

            //Ladder comparator to seek within ranges, could directly cast to char but that defeats the point of weighted averaging
            if(total >= c0 && total < c1) {
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

int CAVE::getLevel() const {
    return this->level;
}

void CAVE::init(std::string seed, int l) {
    this->level = l;
    this->seed = std::move(seed);
}
