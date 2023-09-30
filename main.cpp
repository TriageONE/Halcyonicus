
//#include "lib/world/chunk.h"
//#include "lib/hdb/hdb.h"


#include <iostream>
#include "lib/world/block.h"
#include "lib/world/chunk.h"
#include "lib/hdb/hdb.h"

int main(int argc, char* argv[])
{

    auto *chunk = new CHUNK, *chunk2 = new CHUNK;

    chunk->location = {1, 5};
    chunk2->location = {1, 5};



    //create a checkerboard pattern
    for (int x = 0; x < 63; x++)
        for (int y = 0; y < 63; y++)
            chunk->layers[0].heights[x][y] = (y + (x % 2) ) % 5;

    for (int x = 0; x < 63; x++){
        for (int y = 0; y < 63; y++){
            std::cout << chunk->layers[0].heights[x][y] << " ";
        }
        std::cout << std::endl;
    }

    chunk->layers[0].changed = true;

    std::vector<CHUNK*> chunks {chunk};
    std::vector<CHUNK*> chunks2 {chunk2};
    std::vector<COORDINATE::WORLDCOORD> missingAreas;


    HDB::saveTerrain(&chunks);

    HDB::recallTerrain(&chunks2, &missingAreas);

    for (int x = 0; x < 63; x++){
        for (int y = 0; y < 63; y++){
            std::cout << chunk2->layers[0].heights[x][y] << " ";
        }
        std::cout << std::endl;
    }

    delete chunk, chunk2;

    return 0;

}
