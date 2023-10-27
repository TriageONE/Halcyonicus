//
// Created by Triage on 9/30/2023.
//

#ifndef HALCYONICUS_GENERATOR_H
#define HALCYONICUS_GENERATOR_H

#include "chunk.h"
#include "../noise/perlin.h"
#include "../noise/worley.h"

/**
 * The generator class is responsible for applying the initial worldgen behavior to an entire chunk at a time
 */

using namespace siv;
class GENERATOR{

    template <typename T>
    static T rotateLeft(T value, int shift) {
        static_assert(std::is_integral<T>::value, "Only integral types are supported.");
        constexpr int numBits = sizeof(T) * 8;
        shift = shift % numBits;
        return (value << shift) | (value >> (numBits - shift));
    }

    template <typename T>
    static T rotateRight(T value, int shift) {
        static_assert(std::is_integral<T>::value, "Only integral types are supported.");
        constexpr int numBits = sizeof(T) * 8;
        shift = shift % numBits;
        return (value >> shift) | (value << (numBits - shift));
    }

public:
    /**
     * Generates the entire chunkwide climate for the chunk
     * @param chunk The chunk to modify
     */
    static void genClimate(CHUNK * chunk, auto seed){
        //The climate should adhere to a regional bound where at levels near the equator (near y 0) it is hotter, wile arctic regions are fundamentally colder based on distance from the equator
        /*
         * the distance from the north pole on earth is about 14K miles or 22,530,816 meters.
         * if each block in our world is a meter by a meter, this would mean that players need to travel almost 11 million blocks just to get to the arctic regions. We need to compress this down to
         * a level thats managable, by having the world be fundamentally smaller and more dense. If i were travelling in minecraft, i would expect a distance of 10,000 blocks to be a hike. We should make it so that players
         * can travel to these remote areas and experience the wealth of diversity in climate, so by walking a considerable distance, they would begin to feel the cold embrace of snowfall after wandering too far north
         *
         * The constant shall be 50,000 blocks from the equator to the north, that will be from one zenith to another. The full rotation from one hot or cold zenith shall be 100,000 blocks, or 220x smaller than earthscale.
         * the average temperature in the arctic regions is about -7C. For the desert, it would be about 40C. Average aberration would be about 8C. Day night influence would be about 6C
         */

        const siv::PerlinNoise perlin{rotateLeft(seed, 1)};

        auto loc = chunk->location;
        //Rather than apply the climate sine each time you calculate the temperature, you can speed this up by a few times by calculating a lower resolution value based on the chunk itself instead
        const double sine = ( 100 * sin( (((double) loc.y) / 6000) + (1.570) ) + 118);

        for (short x = 0; x < 16; x++){
            for (short y = 0; y < 16; y++){
                const double noise = perlin.octave2D_01(((double)(loc.x * 16 + x))/ 32, ((double)(loc.y * 16 + y))/ 32, 3);
                chunk->temperature[x][y] = (unsigned char) (sine + ((noise * 8)-4));
            }
        }

        //Output the product in centigrade:
        /*
        for (short x = 0; x <= 15; x++){
            for (short y = 0; y <= 15; y++){
                std::cout << (float) ( ( (130.0 / 255.0) * chunk->temperature[x][y] ) - 70 ) << " ";
            }
            std::cout << std::endl;
        }*/

    }

    static void genTestHeight(CHUNK * chunk, unsigned char layer){
        if (layer < 0 || layer > 24) return;
        for (short x = 0; x < 64; x++) {
            for (short y = 0; y < 64; y++) {
                chunk->layers[layer].heights[x][y] = (short) y;
            }
        }
    }
    /**
     * Generates the proper heightmap for the layer defined where layer 0 is the bottom of the world and layer 23 is the overworld
     * @param chunk
     * @param layer
     */

    static void genHeight(CHUNK * chunk, unsigned char layer, unsigned int seed, float scalar){
        if (layer < 0 || layer > 24) return;
        /*
         * Height is a tricky subject. We would have about 3 layers of noise for three different purposes.
         * Noise that contributes to small abberations to the ground is called roughness, and this should be a further octave onto roughness amount.
         * there will be a primary height control wherein it is a single octave function that outputs the general scheme of the land.
         * The final step is to apply what is known as the metascalar, where we define a larger additive pattern that creates entire continents. without this, we ultimately rely on the main scalar which is too predictable
         */
        const siv::PerlinNoise roughness{rotateRight(seed, 1)};
        const siv::PerlinNoise metaRoughness{rotateRight(seed, 3)};

        const siv::PerlinNoise height{(seed)};
        const siv::PerlinNoise metaHeight{rotateRight(seed, 2)};
        const siv::PerlinNoise metaMetaHeight{rotateRight(seed, 3)};

        auto loc = chunk->location;
        double rNoise, rmNoise, hNoise, mhNoise, mh2Noise;

        for (short x = 0; x < 64; x++){
            for (short y = 0; y < 64; y++){
                rNoise = roughness.noise2D_01(((double)(loc.x * 64 + x))/ 8, ((double)(loc.y * 64 + y))/ 8);
                rmNoise = metaRoughness.noise2D_01(((double)(loc.x * 64 + x))/ 8, ((double)(loc.y * 64 + y))/ 8);
                hNoise = height.octave2D_01(((double)(loc.x * 64 + x))/ 24, ((double)(loc.y * 64 + y))/ 24, 4);
                mhNoise = metaHeight.octave2D_01(((double)(loc.x * 64 + x))/ 128, ((double)(loc.y * 64 + y))/ 128, 2);
                mh2Noise = metaHeight.octave2D_01(((double)(loc.x * 64 + x))/ 512, ((double)(loc.y * 64 + y))/ 512, 3);

                //Apply scalar effects to each noise value

                rNoise = rNoise * 4 * (rmNoise * 0.8) ;
                /*
                 * Height should be limited to 0-1024, but in reality, we should have more margins for actual height
                 * First, the height should never naturally exceed lower than 32, as our oceans should only go so far down
                 * Second, our overworld sea level should be at 128. Anything lower than this can safely be filled with water
                 * FInally, we should have an average abberation of land discrepency of about 128 on a large scale, meaning for the metascalar at play, we would only generate noise ranging by -128 - 127 and add that to a metascalar
                 *
                 *
                 */
                hNoise = (hNoise * 32) + (mhNoise * 256) + (mh2Noise * 512) + 256;

                chunk->layers[layer].heights[x][y] = (short) (hNoise + rNoise);
            }
        }

    }

    /**
     * Step 2 in the process of chunk generation process, can only accurately occur when height and temperature have been set
     */
    static void genHumidity(CHUNK * chunk, unsigned int seed){

        auto loc = chunk->location;
        //Rather than apply the climate sine each time you calculate the temperature, you can speed this up by a few times by calculating a lower resolution value based on the chunk itself instead

        const double sine = ( sin( (((double) loc.y) / 6000) + 0.5 ));
        //const double noise = perlin.octave2D_01(((double)(loc.x * 16 + x))/ 32, ((double)(loc.y * 16 + y))/ 32, 3);
        siv::PerlinNoise perlinNoise{rotateLeft(seed, 1)};

        for (short x = 0; x < 16; x++){
            for (short y = 0; y < 16; y++){

                /* Height and temperature limit humidity in different ways
                 * As your Y value increases, the potential for more rainfall can occur, capping the lower end of the spectra
                 * As your average altitude increases, the ability to generate moisture decreases, putting a high end cap on the spectra
                 */

                double noise = perlinNoise.octave2D_01(((double)(loc.x * 16 + x))/ 32,  ((double)(loc.y * 16 + y))/ 32, 1) * 255;

                /*
                 * The height factor only applies to the overworld, humidity isnt taken into account for lower levels usually.
                 * take layer 0 and say that the height range is great, about from 64-768, therefore at the maximal end of 768, the max amount of humidity would be 70%. This reaches 100% at 512.
                 * With Y value, use the sine function to help with the scaling, where the minimum could be about 25% precipitation.
                 */

                int x4 = x*4, y4 = y*4;

                int avgHeight = chunk->layers[0].heights[x4][y4];
                avgHeight += chunk->layers[0].heights[x4+1][y4+1];
                avgHeight += chunk->layers[0].heights[x4][y4+1];
                avgHeight += chunk->layers[0].heights[x4+1][y4];

                avgHeight = avgHeight/4;

                int upperLimit = 180 + ( 75 - (75 / 0xFF) * max(avgHeight - 512, 0) );
                //When the Y limit exceeds a certain value, the lower end starts to taper.

                double temp;
                if (sine < 0.0) {
                    temp = 0.0; // Output 0 for input less than 0
                } else if (sine >= 0.0 && sine < 0.2) {
                    // Scale linearly from 0.3 to 0.0 as input goes from 0.0 to 0.2
                    temp = 0.3 - (sine / 0.2) * 0.3;
                } else if (sine >= 0.2 && sine <= 0.8) {
                    temp = 0.0; // Output 0 for input in the range [0.2, 0.8]
                } else if (sine > 0.8 && sine <= 1.0) {
                    // Scale linearly from 0.0 to 0.3 as input goes from 0.8 to 1.0
                    temp = (sine - 0.8) / 0.2 * 0.3;
                } else {
                    temp = 0.3; // Output 0.3 for input greater than 1.0
                }
                
                auto lowerLimit = (unsigned char)(temp * 255);

                noise = (((upperLimit) < (noise)) ? (upperLimit) : (noise));
                noise = (((lowerLimit) > (noise)) ? (lowerLimit) : (noise));
                chunk->humidity[x][y] = (unsigned char) noise;
            }
        }
    }
    /**
     * Generates the terrain detail that manifests unto tops of blocks. <br>REMINDER: <br>
     * - The type of the top block can be of 256 different types <br>
     * - The descriptor contains information about the depth of soil at this level<br>
     * - the topper can have a state that allows us to effectively have 64k unique states for our top block<br>
     * @param chunk the chunk to modify
     * @param layer the layer to modify
     */
    static void genTerrainDetail(CHUNK * chunk, unsigned char layer, unsigned int seed){

        auto invert = seed ^ UINT_MAX;
        const siv::PerlinNoise perlin{invert};

        for (short x = 0; x < 64; x++) {
            for (short y = 0; y < 64; y++) {
                //Start with the stopping point of hard stone as a short, based on height and modified by simple noise
                //The soft earth should never extend lower than 32 blocks and is biased upwards 2 blocks

                double noise = (((((perlin.noise2D_01(x, y) * 32) -2)) < (0)) ? (((perlin.noise2D_01(x, y) * 32) -2)) : (0));
                //Since the descriptor is a number denoting the tallness of our stone column where everything above that is dirt, we can take the height and subtract this number from it
                chunk->layers[layer].descriptor[x][y] = chunk->layers[layer].heights[x][y] - (int) noise;


                //Now we can decide the type of block
                //for now we leave it all 0 because that's dirt
            }
43;      }
    }


};
#endif //HALCYONICUS_GENERATOR_H
