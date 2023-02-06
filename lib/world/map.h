//
// Created by Triage on 2/3/2023.
//

#ifndef HALCYONICUS_MAP_H
#define HALCYONICUS_MAP_H

#include <array>
#include <string>

/**
* 0:0, In the beginning, god said, let there be a platform, nothing more. All is up for interpretation, interpolation, and subjection;
* And so he created a class of objects to hold the universe, all in data, no matter how large or small.
* It shall describe the universe as we see it, and contain secrets unknown.
*/
class MAP{
private:
    std::array<char8_t, 1024> map {0};
    int size = sizeof(map) / sizeof(map[0]);
    float vBias=0, scalar=0, roughness=0;
    float w0=1, w1=1, w2=1, w3=1;
    std::string seed;
public:
    explicit MAP() = default;;

    //God says unto 0:2:5 : "Let there be a way for those who manipulate the map at a granular or programmable level to easily interface, as reinventing the wheel is pointless and wastes your time"
    static char8_t compress(std::array<char8_t, 4>);
    static std::array<char8_t, 4> uncompress(char8_t c);

    //God says unto 0:2:2 : "Let there be a way for those such as yourself and your consort to work upon the map, as if it were a coordinate plane read like a book. We shall find the correct placement for you"
    void set(char8_t h, int x, int y);
    char8_t get(int x, int y);

    //God says unto 0:2:3 : "Let there be a way for those to see, one way in memory, and another in a monofaregraph, so that we may understand the root of our product"
    void dump_map();
    void out();

    //God says unto 0:2:4 : "And so we shall let the user poke and prod, as they know what is best and why. With only their intelligence required, the power is theirs to have"
    std::array<char8_t, 1024> copy_map();
    void set_heightmap(std::array<char8_t, 1024> new_map);

    char8_t pick(int place);
    void pack(int place, char8_t value);

    //////////////////////////////
    // Generator info section

    // Setters
    void set_seed(std::string seed);
    void set_vBias(float vBias);
    void set_scalar(float scalar);
    void set_roughness(float roughness);

    void set_w0(float w0);
    void set_w1(float w1);
    void set_w2(float w2);
    void set_w3(float val);

    //Getters
    [[nodiscard]] std::string get_seed() const;
    [[nodiscard]] float get_vBias() const;
    [[nodiscard]] float get_scalar() const;
    [[nodiscard]] float get_roughness() const;

    [[nodiscard]] float get_w0() const;
    [[nodiscard]] float get_w1() const;
    [[nodiscard]] float get_w2() const;
    [[nodiscard]] float get_w3() const;

    //Operators
    void generate();

};


#endif //HALCYONICUS_MAP_H
