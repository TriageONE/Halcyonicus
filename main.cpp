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

    WORLD world = *new WORLD;

    const siv::PerlinNoise::seed_type seed = 141249u;
    const siv::PerlinNoise perlin{ seed };

    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            const int noise = (int) (perlin.octave2D_01((x * 0.04), (y * 0.04), 2) * 4);
            world.heightmap.set((char8_t) noise, x, y);
        }

    }
    world.heightmap.out();
    cout << endl;

    std::array<char8_t, 1024> pMap = world.heightmap.copy_map();

    std::array<char8_t, 4> h = MAP::uncompress(pMap[0]);
    cout << "INITIAL: ";
    for (int i = 0; i <4; i++){
        char t = (char)h[1];
        cout << t;
    }
    cout << endl;

    h[0] = 0;
    h[1] = 1;
    h[2] = 2;
    h[3] = 3;
    cout << "AFTER: ";
    for (int i = 0; i <4; i++){
        cout << (char)h[i];
    }
    cout << endl;
    pMap[0] = MAP::compress(h);

    cout << "SANITY: ";
    array<char8_t, 4> arr = MAP::uncompress(pMap[0]);
    for (int i = 0; i <4; i++){
        cout << (char)arr[i];
    }
    world.heightmap.set_heightmap(pMap);

    world.heightmap.dump_map();
    system("Pause");
    return 0;

}
