//
// Created by Triage on 2/3/2023.
//

#ifndef HALCYONICUS_MAP_H
#define HALCYONICUS_MAP_H

#include <array>
#include <string>
#include "worldcoord.h"

/**
* 0:0, In the beginning, god said, let there be a platform, nothing more. All is up for interpretation, interpolation, and subjection;
* And so he created a class of objects to hold the universe, all in data, no matter how large or small.
* It shall describe the universe as we see it, and contain secrets unknown.
*/
class MAP{

protected:
    /**
     * The master array of packed double bit values. A Duo is a unit of data exactly 2 bits wide.
     * A char is always 8 bits wide, therefore 4 duos an be fit within one char, creating an actualized array of 4096 unique entities
     */
    std::array<char, 1024> map {0};

    //The size of the map should always be exactly 1024, however there could be an instance where this becomes larger if we were using a texas instruments calculator to run the server.
    //TI devices use 16 bit words instead of 8, meaning that this size of would calculate to 2048 which is incorrect and therefore compatible, but inefficient.
    int size = sizeof(map);

    //Values for calculating the functional bias towards being slightly higher or lower (Vbias), the scale of the map (Scalar) and the randomness (roughness)
    float vBias=0, scalar=0.02, roughness=0;
    //Values for denoting the weights of each attribute
    float w0=1, w1=1, w2=1, w3=1;
    //The seed of the world held carefully within a string
    std::string seed{};
    //The location of this map
    WORLDCOORD l{};
    //The control variables for determining if this area is filled out or not
    bool generated=false, initialized=false;
public:
    /**
     * The default constructor for the map that initializes a blank map and no special values
     */
    explicit MAP() = default;

    //God says unto 0:2:5 : "Let there be a way for those who manipulate the map at a granular or programmable level to easily interface, as reinventing the wheel is pointless and wastes your time"
    /**
     * Given an array of 4 Duos as chars, compress them into one char in order of which they were received.
     * @return a compression of 4 duos
     */
    static char compress(std::array<char, 4>);

    /**
     * Given one char, compress them into an array of 4 Duos as chars in order of which they were received.
     * @param c a single compressed char
     * @return an uncompressed array of duos
     */
    static std::array<char, 4> uncompress(char c);

    //God says unto 0:2:2 : "Let there be a way for those such as yourself and your consort to work upon the map, as if it were a coordinate plane read like a book. We shall find the correct placement for you"
    /**
     * Sets the duo within the array given a specific coordinate.
     * @param h a duo object as a char to be placed within the map
     * @param x The horizontal component of the world by tile
     * @param y The vertical component of the world by tile
     */
    void set(char h, int x, int y);

    /**
     * Gets the duo within the array given a specific coordinate.
     * @param x The horizontal component of the world by tile
     * @param y The vertical component of the world by tile
     */
    char get(int x, int y);

    //God says unto 0:2:3 : "Let there be a way for those to see, one way in memory, and another in a monofaregraph, so that we may understand the root of our product"
    /**
     * A special function to output the raw binary data to the console line by line.
     */
    void dump_map();
    /**
     * A special function to draw the map within console in greyscale unicode box characters. The algorithm used outputs 2 boxes per area, since unicode boxes usually occupy a ratio of 2:1 height to width.
     */
    void out();

    //God says unto 0:2:4 : "And so we shall let the user poke and prod, as they know what is best and why. With only their intelligence required, the power is theirs to have"
    /**
     * Grabs a copy of the map
     * @return The copy of the map
     */
    std::array<char, 1024>  copy_map();
    /**
     * Sets the passed map to this object, overriding the entire map
     * @param new_map The new map provided
     */
    void set_heightmap(std::array<char, 1024>  new_map);

    /**
     * given a place raw within the duo map array sized at 4096 elements, get the value at that place as a duo
     * @param place the place of the array from 0-4095
     * @return the duo at that place
     */
    char pick(int place);
    /**
     * Pack the duo passed into the place in the array.
     * @param place the place of the array from 0-4095
     * @param value the duo at that place
     */
    void pack(int place, char value);

    /**
     * Get a raw compressed quad from the array
     * @param place the place of the array from 0-1023
     * @return the quad at that place containing 4 duos
     */
    char getRaw(int place);
    /**
     * Set a raw compressed quad into the array
     * @param place the place of the array from 0-1023
     * @param value the quad containing 4 duos
     */
    void setRaw(int place, char value);

    //////////////////////////////
    // Generator info section

    // Setters

    void setSeed(std::string seed);
    void setLocation(WORLDCOORD worldcoord);

    virtual void setVBias(float vBias);
    void setScalar(float scalar);
    void setRoughness(float roughness);

    void setW0(float w0);
    void setW1(float w1);
    void setW2(float w2);
    void setW3(float val);

    //Getters
    [[nodiscard]] std::string getSeed() const;
    [[nodiscard]] WORLDCOORD getWorldCoord() const; //This is intrinsic to the region format and may be removed,
    [[nodiscard]] float getVBias() const;
    [[nodiscard]] float getScalar() const;
    [[nodiscard]] float getRoughness() const;

    [[nodiscard]] float getW0() const;
    [[nodiscard]] float getW1() const;
    [[nodiscard]] float getW2() const;
    [[nodiscard]] float getW3() const;

    //Operators
    virtual void generate();
    [[nodiscard]] bool isInitialized() const;
    [[nodiscard]] bool isGenerated() const;
    void init(std::string seed, WORLDCOORD worldcoord);
    int getHash();

    std::string getRawHash();
};


#endif //HALCYONICUS_MAP_H
