#include <iostream>
#include <fstream>
#include "lib/world/world.h"
#include "lib/noise/perlin.h"
#include "lib/world/region.h"
#include "lib/world/heightmap.h"

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
    int matches = 0, misses = 0;

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

            //w.out();

            REGIONCOORD rc = wc.getRegionCoordinates();
            cout << "Region " << rc.getX() << ", " << rc.getY() << endl;
            cout << "WorldShard " << wc.getX() << ", " << wc.getY() << endl;

            if (existenceCache.empty()){
                cout << "Testing to see if the region " << rc.getX() << ", " << rc.getY() << " exists.." << endl;
                bool exists = REGION::regionExists(wc.getRegionCoordinates());
                filesystem::path path = REGION::parseFullPathFromRegionCoord(wc.getRegionCoordinates());
                if (!exists){
                    cout << "Creating empty world with path " << path << endl;
                    REGION::createEmptyWorld(path);
                }
                existenceCache.push_back(rc);
            }
            for (REGIONCOORD r : existenceCache){
                int x1,y1;
                x1=r.getX();
                y1=r.getY();
                if (rc.getY() == y1 && rc.getX() == x1){
                    cout << "Testing to see if the region " << x1 << ", " << y1 << " exists.." << endl;
                    bool exists = REGION::regionExists(wc.getRegionCoordinates());
                    filesystem::path path = REGION::parseFullPathFromRegionCoord(wc.getRegionCoordinates());
                    if (!exists){
                        cout << "Creating empty world with path " << path << endl;
                        REGION::createEmptyWorld(path);
                        existenceCache.push_back(rc);
                    }

                }
            }

            string w1h, w2h;

            w1h = w.getRawHash();
            REGION::writeChunk(&w);

            WORLD w2(seed, wc);
            REGION::readChunk(&w2);
            w2h = w2.getRawHash();

            cout << "##################\nCOMMENCE READ OUTPUT\n##################" << endl;

            //w2.out();

            cout << "WC_" << x << "_" << y << ": \n\t" << w1h << ", \n\t" << w2h << ";\n\t" << ((w1h == w2h) ? "MATCH" : "NO_MATCH") << endl;
            if (w1h == w2h) matches++; else misses++;
        }
    }

    cout << "JOB FINISHED, MATCHES:" << matches << ", MISSES: " << misses << endl;
    return 0;

}
