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

    world.heightmap.set_scalar(0.01);
    world.climatemap.set_scalar(0.03);
    world.generate();


    world.heightmap.out();
    cout << endl;
    world.climatemap.out();
    system("Pause");
    return 0;

}
