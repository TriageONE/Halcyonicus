

#include <vector>
#include "lib/entity/entity.h"
#include "lib/world/block.h"


int main(int argc, char* argv[])
{

    std::cout << "beginning block construction" << std::endl;
    BLOCK b1;
    b1.type=60291;
    b1.orientation = BLOCK::ORIENTATION::EAST;
    b1.location = {123, 525, 2333};

    char blk[9];

    b1.serialize(blk);

    BLOCK b2;
    b2.deserialize(blk);
    b1.out();
    b2.out();


    return 0;
}
