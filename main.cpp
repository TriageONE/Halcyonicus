#include <iostream>
#include "lib/world/world.h"
#include "lib/noise/perlin.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <Windows.h>
#include <cstdio>
#endif

using namespace std;

int main() {

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        SetConsoleOutputCP(CP_UTF8);
        // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
        setvbuf(stdout, nullptr, _IOFBF, 2000);
    #endif

    auto world = WORLD("bingus");
    MAP hmap = world.heightmap;

    // Larger values make more complexity
    hmap.set_scalar(0.04);
    // Larger values make for more variation and random noise
    hmap.set_roughness(0.1);
    // The W class decides the weights of each tile selection, and higher values against lower values in general bias those types of tiles to generate
    hmap.set_w0(3);
    hmap.set_w1(1);
    hmap.set_w2(3);
    hmap.set_w3(1);
    // Generates the map
    hmap.generate();
    // Let me see what it did
    hmap.out();
    cout << endl;
    return 0;

}
