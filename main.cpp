#include <iostream>
#include "lib/world/world.h"

using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    HEIGHTMAP map = *new HEIGHTMAP;
    map.set(HEIGHT::LAND, 63, 63);
    cout << map.get(63, 63) << endl;
    cout << map.get(62, 63) << endl;
    cout << map.get(0, 0) << endl;
    return 0;
}
