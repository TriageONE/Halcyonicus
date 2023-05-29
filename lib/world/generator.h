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
        float scalar = 0.05;
        float roughness = 0;
        float vBias = 0;
    };

    static void applyPhase1(GEN_OPTIONS options, chunk *chunk){
        // Given a seed, we should be able to downfill with perlin noise and entire chunk's worth of stone into the chunk
        int s1 = *(int*) (options.seed.c_str());
        std::cout << "   #MAPGEN S1 " << s1 << std::endl;

        int s2 = ((*(int*) options.seed.c_str()) << 1) + 16;
        std::cout << "   #MAPGEN S2 " << s2 << std::endl << std::endl;

        const siv::PerlinNoise::seed_type pseed = s1;
        const siv::PerlinNoise perlin{ pseed };
        const siv::PerlinNoise::seed_type sseed = s2;
        const siv::PerlinNoise mperlin{ sseed };

        float metascalar = options.scalar/4 < 0.0001f ? 0.0001f : options.scalar/4;
        metascalar = metascalar > 0.04f ? 0.04f : metascalar;
        int xoff, yoff;
        xoff = chunk->chunkX * 16;
        yoff = chunk->chunkZ * 16;
        //Repeat for each tile within the map
        for (int z = 0; z < 16; ++z)
        {
            for (int x = 0; x < 16; ++x)
            {
                //The metascalar is a method to bias the entire noise pattern to create large metapatterns.
                //This prevents static patterns that reoccur over a large area, such as creating a map where oceans really don't exist, because the granularity is too low.
                //This is the second abstraction layer to design a trend around a larger context, like biomes wrap landmasses, so that not all landmasses are the same
                double metamap = (mperlin.octave2D_01((x * metascalar) + xoff, (z * metascalar) + yoff, 2) * 2 ) - 1;

                //Roughness is a measure of variation, but not random. It must also be seeded perlin noise, transforming the major perlin noise product
                double rnoise = 0.0;
                if (options.roughness != 0.0f)
                    //The roughness map is an inversely correlated, static scalar, single octave perlin noise mapping that relies on the pattern of the main world.
                    //I'm not sure, but I think if we were to render a map at 0.5 scalar with no vbias and any roughness between 0.25-0.50, we may get a pretty looking 'reflection' across the world axi
                    rnoise = ((perlin.octave2D_01(((-1) * (x * 0.5)) + xoff, ((-1) * (z * 0.5)) + yoff, 1) * 2) - 1 ) * options.roughness;
                //VBias is a method if adding a constant height decrement or increment so that the likelihood of generating an ocean for example would increase linearly
                //Scalar is also important and allows for you to kinda "zoom in or out" on the projection
                double noise = perlin.octave2D_01((x * options.scalar) + xoff, (z * options.scalar) + yoff, 2) + options.vBias + rnoise + metamap / 4;
                //TODO: Affect the column with the new values all in one go
                int expandedValue = (noise * 186) + 38;

                for (short i = expandedValue; i > 0; i--){
                    chunk->changeCube(x, i, z, 1, true);
                }
            }
        }
        chunk->recompileAll();
    }
};

#endif //HALCYONICUS_GENERATOR_H
