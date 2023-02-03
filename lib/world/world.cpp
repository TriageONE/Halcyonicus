//
// Created by Triage on 1/31/2023.
//

#include "world.h"
#include <iostream>
#include "array"

//////////////////////////
//CLASSIFICATION BASIC RW
//////////////////////////
/**
 * <h1> Function SET </h1>
 * <p><i>
 * "And so a way was needed, one that could implement individual data onto the array.
 * <br>
 * The ideal user would immediately want for a method that allows them to set a value, one at a time"
 * </i></p>
 *
 * @param h Height, an enumerated value granted by world.h, where 1 of 4 values can be used.
 * <ul>
 *      <li>OCEAN, (0)
 *      <li>WATER, (1)
 *      <li>LAND, (2)
 *      <li>MOUNTAIN (3)
 * </ul>
 *
 * @param x
 *  an integer defining the relative X coordinate of the selected tile to manipulate
 * @param y
 *  an integer defining the relative Y coordinate of the selected tile to manipulate
 *
 * <h2>RELATIVISTIC COORDINATE DISAMBIGUATION:</h2>
 * <p>
 *  This coordinate plane is independent to itself. It does not call to the greater context of the world as we know it around the map.
 * </p><p>
 *  In a coordinate grid, UP and to the RIGHT is the section most positive, however a balance must be struck, and therefore DOWN and to the RIGHT is recommended to align to
 *  The reasoning behind this is actually because it would be the way we read. We read LTR and descending. This ensures our grid is readable like a book. Therefore,
 *  </p>
 *  <pre>
 *          \<- X ->
 *  <br> 0, 0 -------------- 63, 0
 *  <br> |                       |
 *  <br> |                       |  ^
 *  <br> |                       |  |
 *  <br> |                       |  Y
 *  <br> |                       |  |
 *  <br> |                       |  V
 *  <br> |                       |
 *  <br> 0, 63 ------------ 63, 63 </pre>
 */
void HEIGHTMAP::set(HEIGHT h, int x, int y) {
    //And thus a safeguard was ensured, such that exiles shall not exceed ever the place of the master array
    x = std::clamp(x, 0, 63);
    y = std::clamp(y, 0, 63);
    //Finally, a shift was used, as multiplication is not needed, and the compiler should have an easier life by not having to use its optimizer;
    int place = x + (y << 6);

    pack(place, h);
}

/**
 * <h1>Function GET</h1>
 * <p>
 * Gets a specific HEIGHT value from the HEIGHTMAP defined.
 * Given a relative coordinate, reading LTR (-->), retrieve the respective HEIGHT from that specific spot
 * </p>
 * <p>
 * All coordinates are clamped to 0...63. This means any attempt to feed the system data that exceeds either bound, negative or positive, will be transformed into the closest clamp value, either 0 or 63.
 * </p>
 *
 * @param x The X part from the relative coordinate, reading LEFT to RIGHT, where 0 aligns to the far LEFT and 63 reaches to the FAR RIGHT
 * @param y The Y part from the relative coordinate, reading TOP to BOTTOM, where a 0 aligns to the TOPMOST row, and 63 reaches the BOTTOM-MOST row.
 * @return The respective HEIGHT value gathered at that area
 *
 * <h2>RELATIVISTIC COORDINATE DISAMBIGUATION:</h2>
 * <p>
 *  This coordinate plane is independent to itself. It does not call to the greater context of the world as we know it around the map.
 * </p><p>
 *  In a coordinate grid, UP and to the RIGHT is the section most positive, however a balance must be struck, and therefore DOWN and to the RIGHT is recommended to align to
 *  The reasoning behind this is actually because it would be the way we read. We read LTR and descending. This ensures our grid is readable like a book. Therefore,
 *  </p>
 *  <pre>
 *          \<- X ->
 *  <br> 0, 0 -------------- 63, 0
 *  <br> |                       |
 *  <br> |                       |  ^
 *  <br> |                       |  |
 *  <br> |                       |  Y
 *  <br> |                       |  |
 *  <br> |                       |  V
 *  <br> |                       |
 *  <br> 0, 63 ------------ 63, 63 </pre>

 *
 */
HEIGHT HEIGHTMAP::get(int x, int y) {
    //And thus a safeguard was ensured, such that exiles shall not exceed ever the place of the master array
    x = std::clamp(x, 0, 63);
    y = std::clamp(y, 0, 63);
    //Finally, a shift was used, as multiplication is not needed, as the optimizer is heresy and shall be ignored when possible.
    int place = x + (y << 6);
    HEIGHT picked = pick(place);
    return picked;
}

//////////////////////////////////////
//CLASSIFICATION RAW MAP MANIPULATION
//////////////////////////////////////
/**
 * <h1>Function COPY_MAP</h1>
 * A function built to return a raw operable array of HEIGHTs. This is still encoded into its primal form, compacted, with each char representing 4 tiles at once.
 * @return a char array copied from the instantaneous value present of this object's map
 */
std::array<unsigned char, 1024> HEIGHTMAP::copy_map() {
    std::array<unsigned char, 1024> copy = {};
    int size = sizeof(map)/sizeof(map[0]);

    for (int i = 0; i < size; i++){
        copy[i] = map[i];
    }
    return copy;
}

/**
 * <h1>Function SET_MAP</h1>
 * Overwrites the entire map array
 * @param array The new array to write onto this object's map
 */
void HEIGHTMAP::set_map(std::array<unsigned char, 1024> new_map){
    int size = sizeof(map)/sizeof(map[0]);

    for (int i = 0; i < size; i++){
        map[i] = new_map[i];
    }
}
//////////////////////////////////
//CLASSIFICATION DATA COMPRESSION
//////////////////////////////////
/**
 * <h1>Function UNCOMPRESS</h1>
 * Uncompresses the char back into an array of HEIGHTS to be manipulated by another routine or function
 * @param c CHAR, The char that should be converted to an uncompressed character array
 * @return an array of uncompressed data containing 4 unique tiles within the heightmap
 */
std::array<HEIGHT, 4> HEIGHTMAP::uncompress(unsigned char c){
    std::array<HEIGHT, 4> out = {};
    for (int i = 0; i < 4; i++){
        out[i] = (HEIGHT) (c & 3);
        c = c >> 2;
    }
    return out;
}

/**
 * <h1>Function COMPRESS</h1>
 * Compresses the array of heights back into a char to be placed within the master graph
 * @param in ARRAY<HEIGHT, len 4>, The array that should be converted to a compressed character
 * @return a character of compressed data containing 4 unique tiles within the heightmap
 */
unsigned char HEIGHTMAP::compress(std::array<HEIGHT, 4> in){
    unsigned char out = 0;
    for (char i = 0; i < 4; i++){
        out = (in[i] << (i * 2)) | out;
    }
    return out;
}

////////////////////////////////////
//CLASSIFICATION ARRAY MANIPULATION
////////////////////////////////////
/**
 * <h1>Internal Function PICK</h1>
 * <p>
 * A private function that is meant to perform the bitwise uncompression needed to extract the correct two bits from the master array.
 * </p>
 * @param place INTEGER, A value ranging 0...4095, where this denotes the actual location within the master array for what bit pair to extract. This value is clamped so that it may not exceed 4095 or 0.
 * @return HEIGHT, An enumeration that denotes the height of that array element, ranging from 0...3 as enumerable.
 */
HEIGHT HEIGHTMAP::pick(int place) {

    place = std::clamp(place, 0, 4095);
    unsigned int cSpace = place >> 2;
    unsigned char cBit = map[cSpace];
    std::array<HEIGHT, 4> a = uncompress(cBit);
    return a[place % 4];
}

/**
 * <h1>Internal Function PACK</h1>
 * <p>
 * A private function that is meant to perform the bitwise overwrite needed to implement the correct two bits to the master array.
 * </p>
 * @param place INTEGER, A value ranging 0...4095, where this denotes the actual location within the master array for what bit pair to extract. <br>
 * This value is clamped so that it may not exceed 4095 or 0.
 * @param value HEIGHT, An enumeration that denotes the height of that array element, ranging from 0...3 as enumerable.
 */
void HEIGHTMAP::pack(int place, HEIGHT value) {
    place = std::clamp(place, 0, 4095);
    unsigned int cSpace = place >> 2;
    unsigned char cBit = map[cSpace];

    std::array<HEIGHT, 4> a = uncompress(cBit);
    a[place % 4] = value;
    map[cSpace] = compress(a);
}

/////////////////////////
// CLASSIFICATION: OUTPUT
/////////////////////////
/**
 * <h1>Function DUMP_MAP</h1>
 * Used to print each individual int value as an integer to the console delimited by semicolons.
 */
void HEIGHTMAP::dump_map(){
    int size = sizeof(map)/sizeof(map[0]);
    std::cout << "DUMPING MAP SIZE (" << size << ") : ";

    for (int i = 0; i < size; i++){
        if (i % 32 == 0) std::cout << std::endl << "#: ";
        std::cout << std::bitset<8> (map[i]) << "; ";
    }
    std::cout << std::endl << "MAP DUMPING COMPLETE " << std::endl;
}

/**
 * <h1>Function OUT</h1>
 * A function dedicated to visualizing the array as it currently is, within console, with varying ASCII blocks uncolored to represent the heightmap as seen from a birds-eye-view
 */
void HEIGHTMAP::out(){
    // Outputting 4096 values in a 64x64 array
    int place = 0;
    int line = 0;
    for (int y = 0; y < 64; ++y)
    {
        std::cout << std::endl << "#" << line << ":  \t" ;
        line++;
        for (int x = 0; x < 64; ++x)
        {
            HEIGHT h = pick(place);
            place++;
            switch(h){
                case HEIGHT::OCEAN:
                    std::cout << "░░";
                    break;
                case HEIGHT::WATER:
                    std::cout << "▒▒";
                    break;
                case HEIGHT::LAND:
                    std::cout << "▓▓";
                    break;
                case HEIGHT::MOUNTAIN:
                    std::cout << "██";
                    break;
            }
        }

    }
}







