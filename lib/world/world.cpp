//
// Created by Triage on 1/31/2023.
//
#include "world.h"
#include <iostream>

/**
 * And so a way was needed, one that could implement individual data onto the array.
 * The ideal user would immediately want for a method that allows them to set a value, one at a time
 *
 * INPUTS:
 * HEIGHT h:
 *  an enumerated value granted by world.h, where 1 of 4 values can be used.
 *      OCEAN, (0)
 *      WATER, (1)
 *      LAND, (2)
 *      MOUNTAIN (3)
 *
 * int x:
 *  an integer defining the relative X coordinate of the selected tile to manipulate
 * int y:
 *  an integer defining the relative Y coordinate of the selected tile to manipulate
 *
 * RELATIVISTIC COORDINATE DISAMBIGUATION:
 *  This coordinate plane is independent to itself. It does not call to the greater context of the world as we know it around the map
 *  as such, we can interpret this heightmap as a grid unto itself and not modified by esternal factors or placements.
 *
 *  In a coordinate grid, UP and to the RIGHT is the section most positive, however a balance must be struck, and therefore DOWN and to the RIGHT is recommended to align to
 *  The reasoning behind this is actually because it would be the way we read. We read LTR and descending. This ensures our grid is readable like a book. Therefore,
 *          <- X ->
 *  0, 0 ------------- 63, 0
 *  |                       |
 *  |                       |  ^
 *  |                       |  |
 *  |                       |  Y
 *  |                       |  |
 *  |                       |  V
 *  |                       |
 *  0, 63 ------------ 63, 63
 */

void HEIGHTMAP::set(HEIGHT h, int x, int y) {
    //And thus a safeguard was ensured, such that exiles shall not exceed ever the place of the master array
    x = std::clamp(x, 0, 63);
    y = std::clamp(y, 0, 63);
    //Finally, a shift was used, as multiplication is not needed, and the compiler should have an easier life by not having to use its optimizer;
    int place = x + (y << 6);

    //TODO: MUST REMOVE WITH METAFUNCTION
    map[place] = h;
    std::cout << "Placed " << h << " at place " << place << std::endl;
    //
}

HEIGHT HEIGHTMAP::get(int x, int y) {
    //And thus a safeguard was ensured, such that exiles shall not exceed ever the place of the master array
    x = std::clamp(x, 0, 63);
    y = std::clamp(y, 0, 63);
    //Finally, a shift was used, as multiplication is not needed, and the compiler should have an easier life by not having to use its optimizer;
    int place = x + (y << 6);

    //TODO: MUST REMOVE WITH METAFUNCTION
    return this->map[place];
}

HEIGHT HEIGHTMAP::pick(int place) {
    int cSpace = place >> 3;
    int alignedPlace = cSpace << 3;
    char cBit = map[cSpace];
    int offset = place - alignedPlace;
    unsigned char mask;
    switch (offset){
        case 0: mask = 0b11000000;
        case 1: mask = 0b00110000;
        case 2: mask = 0b00001100;
        default: mask = 0b00000011;
    }
    
    return WATER;
}

void HEIGHTMAP::pack(int place, HEIGHT value) {

}






