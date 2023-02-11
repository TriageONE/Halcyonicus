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

    LOCATION l = LOCATION(0,0,0);
    WORLD w = WORLD("lole", l);
    w.constrain();
    MAP *h = w.getHeightmap();
    //today i learned the diff between -> and .
    //one is for pointers, one is for members
    //pointers are fun and easy to use, and you should use them more
    //If you dont use pointers, you will copy data rather than get the data


    //Larger values result in a more zoomed out display
    h->setScalar(0.1);
    h->setRoughness(0.02);
    w.generate();
    h->out();

    // Larger values make more complexity
    cout << endl;

    return 0;

}
