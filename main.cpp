#include <iostream>
#include <fstream>
#include "lib/world/world.h"
#include "lib/noise/perlin.h"
#include "lib/world/region.h"

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

    string seed = "hyperion";

    if(!REGION::checkForDirectoryStructure()){
        cout << "Creating directories..." << endl;
        REGION::createDirectories();
    }

    vector<REGIONCOORD> existenceCache;

    for (int x = 0; x < 16; x++){
        for (int y = 0; y < 16; y++){
            WORLDCOORD wc = WORLDCOORD(x,y,0);
            WORLD w = WORLD(seed, wc);
            w.constrain();
            MAP *h = &w.heightmap;
            //Larger values result in a more zoomed out display
            h->setScalar(0.0075);
            h->setRoughness(0.08);

            w.generate();

            REGIONCOORD rc = wc.getRegionCoordinates();
            for (REGIONCOORD r : existenceCache){
                int x1,y1;
                x1=r.getX();
                y1=r.getY();
                if (rc.getY() == y1 && rc.getX() == x1){
                    bool exists = REGION::regionExists(wc.getRegionCoordinates());

                    if (!exists){
                        filesystem::path path = REGION::parseFullPathFromRegionCoord(wc.getRegionCoordinates());
                        cout << "Creating empty world with path " << path << endl;
                        REGION::createEmptyWorld(path);
                    }
                    existenceCache.push_back(rc);
                }
            }


            string w1h, w2h;

            w1h = w.getRawHash();
            REGION::writeChunk(&w);

            WORLD w2(seed, wc);
            REGION::readChunk(&w2);
            w2h = w2.getRawHash();

            cout << "WC_" << x << "_" << y << ": \n\t" << w1h << ", \n\t" << w2h << ";\n\t" << ((w1h == w2h) ? "MATCH" : "NO_MATCH") << endl;
        }
    }










    return 0;

}
