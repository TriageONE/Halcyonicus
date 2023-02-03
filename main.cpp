#include <iostream>
#include "lib/world/world.h"
#include "lib/noise/perlin.h"

using namespace std;

int main() {
    HEIGHTMAP map = *new HEIGHTMAP;

    const siv::PerlinNoise::seed_type seed = 141249u;
    const siv::PerlinNoise perlin{ seed };

    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            const int noise = (int) (perlin.octave2D_01((x * 0.04), (y * 0.04), 2) * 4);
            map.set((HEIGHT) noise, x, y);
        }

    }
    map.out();
    cout << endl;

    std::array<unsigned char, 1024> pMap = map.copy_map();

    std::array<HEIGHT, 4> h = HEIGHTMAP::uncompress(pMap[0]);
    cout << "INITIAL: ";
    for (int i = 0; i <4; i++){
        cout << h[i];
    }
    cout << endl;

    h[0] = HEIGHT::OCEAN;
    h[1] = HEIGHT::WATER;
    h[2] = HEIGHT::LAND;
    h[3] = HEIGHT::MOUNTAIN;
    cout << "AFTER: ";
    for (int i = 0; i <4; i++){
        cout << h[i];
    }
    cout << endl;
    pMap[0] = HEIGHTMAP::compress(h);

    cout << "SANITY: ";
    array<HEIGHT, 4> arr = HEIGHTMAP::uncompress(pMap[0]);
    for (int i = 0; i <4; i++){
        cout << arr[i];
    }
    map.set_map(pMap);

    map.dump_map();
    return 0;
}
