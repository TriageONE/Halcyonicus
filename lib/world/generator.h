//
// Created by Triage on 5/25/2023.
//

#ifndef HALCYONICUS_GENERATOR_H
#define HALCYONICUS_GENERATOR_H

#include <string>
#include <iostream>
#include "../noise/perlin.h"
#include "chunk.h"

class GENERATOR{

public:

    struct GEN_OPTIONS{
        std::string seed = "default";
        float scalar = 0.08;
        float roughness = 0;
        float amplitude = 64;
        int middleGround = 64;
    };

    static void applyPhase1(GEN_OPTIONS options, chunk *chunk){
        // Given a seed, we should be able to downfill with perlin noise and entire chunk's worth of stone into the chunk
        int s1 = *(int*) (options.seed.c_str());

        int s2 = ((*(int*) options.seed.c_str()) << 1) + 16;

        const siv::PerlinNoise::seed_type pseed = s1;
        const siv::PerlinNoise perlin{ pseed };
        const siv::PerlinNoise::seed_type sseed = s2;
        const siv::PerlinNoise mperlin{ sseed };

        float metascalar = options.scalar/4 < 0.0001f ? 0.0001f : options.scalar/4;
        metascalar = metascalar > 0.04f ? 0.04f : metascalar;
        int xoff, zoff;
        xoff = chunk->location.getX()*16;
        zoff = chunk->location.getZ()*16;

        //Repeat for each tile within the map
        for (int z = 0; z < 16; ++z)
        {
            for (int x = 0; x < 16; ++x)
            {
                //The metascalar is a method to bias the entire noise pattern to create large metapatterns.
                //This prevents static patterns that reoccur over a large area, such as creating a map where oceans really don't exist, because the granularity is too low.
                //This is the second abstraction layer to design a trend around a larger context, like biomes wrap landmasses, so that not all landmasses are the same
                double metamap = ((mperlin.octave2D_01((x + xoff) * metascalar, (z + zoff) * metascalar, 2) * 2 ) - 1) / 2.0;

                //Roughness is a measure of variation, but not random. It must also be seeded perlin noise, transforming the major perlin noise product
                double rnoise = ((perlin.octave2D_01((x + xoff), (z + zoff) , 2) * 2) - 1 ) * options.roughness +  0.1;
                //VBias is a method if adding a constant height decrement or increment so that the likelihood of generating an ocean for example would increase linearly
                //Scalar is also important and allows for you to kinda "zoom in or out" on the projection

                double noise = perlin.octave2D_01((x + xoff) * options.scalar, (z + zoff) * options.scalar, 2) * (rnoise + metamap);

                int expandedValue = (noise * options.amplitude) + options.middleGround;
                int dirtAmount = (rnoise * 4) + 2;

                for (short i = expandedValue; i > 0; i--){
                    short type;
                    if (i == expandedValue){
                        type = 1;
                    } else if (i <= expandedValue - dirtAmount ){
                        type = 3;
                    } else
                        type = 2;

                    chunk->changeCube(x, i, z, type, false);
                }
            }
        }
        chunk->recompileAll();
    }
};

#endif //HALCYONICUS_GENERATOR_H
