
//#include "lib/world/chunk.h"
//#include "lib/hdb/hdb.h"


#include <iostream>
#include "lib/world/block.h"
#include "lib/world/chunk.h"
#include "lib/hdb/hdb.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello" << std::endl;

    std::cout << "Beginning block construction" << std::endl;
    BLOCK b1;
    b1.type=60291;
    b1.orientation = BLOCK::ORIENTATION::EAST;
    b1.location = {5, 22, 2333};

    unsigned char blk[9];

    b1.serialize(blk);

    BLOCK b2;
    b2.deserialize(blk);
    b1.out();
    b2.out();

    info << "Looking good so far? lets hope so. B2 should now be at loc offset 22x 52y 525z type 7000 ori WEST" << std::endl;

    b2.location = {22, 52, 525};
    b2.type = 7000;
    b2.orientation = BLOCK::WEST;

    BLOCK b3, b4;

    b3.location = {21, 54, 55};
    b3.type = 7200;
    b3.orientation = BLOCK::UP;

    b4.location = {1, 61, 580};
    b4.type = 7300;
    b4.orientation = BLOCK::SOUTH;

    auto *chunk = new CHUNK, *chunk2 = new CHUNK;

    chunk->location = {1, 5};
    chunk2->location = {1, 5};
    chunk->blocks.push_back(b1);
    chunk->blocks.push_back(b2);
    chunk->blocks.push_back(b3);
    chunk->blocks.push_back(b4);

    HDB::saveBlocks(chunk);

    chunk2->location = {1, 5};

    HDB::recallBlocks(chunk2);
    info << "Recall of blocks completed, output:" << nl;
    for (auto b : chunk2->blocks){
        b.out();
    }
/**/

    return 0;
}
