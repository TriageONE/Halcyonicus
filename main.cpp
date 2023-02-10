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

    auto world = WORLD("kjefiub");

    // Larger values make more complexity
    world.heightmap.setScalar(0.04);
    // Larger values make for more variation and random noise
    world.heightmap.setRoughness(0.1);
    // The W class decides the weights of each tile selection, and higher values against lower values in general bias those types of tiles to generate
    world.heightmap.setW0(3);
    world.heightmap.setW1(1);
    world.heightmap.setW2(3);
    world.heightmap.setW3(1);
    // Generates the map
    world.constrain();
    world.generate();
    // Let me see what it did
    world.heightmap.out();

    cout <<  endl;
    world.climatemap.out();
    cout <<  endl;
    world.saturationmap.out();
    cout <<  endl;

    for (CAVE c : world.caves){
        cout << "CAVE LAYER " << c.getLevel() << endl;
        c.out();
        cout <<  endl;
    }

    return 0;

}
