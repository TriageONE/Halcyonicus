//
// Created by Triage on 2/3/2023.
//

#include "map.h"
#include "../noise/perlin.h"
#include <iostream>
#include <bitset>
#include <array>
#include <utility>

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
 *  This coordinate plane is independent to itself. It does not call to the greater context of the world as we know it around the heightmap.
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
void MAP::set(char8_t h, int x, int y) {
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
 * Gets a specific HEIGHT value from the MAP defined.
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
 *  This coordinate plane is independent to itself. It does not call to the greater context of the world as we know it around the heightmap.
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
char8_t MAP::get(int x, int y) {
    //And thus a safeguard was ensured, such that exiles shall not exceed ever the place of the master array
    x = std::clamp(x, 0, 63);
    y = std::clamp(y, 0, 63);
    //Finally, a shift was used, as multiplication is not needed, as the optimizer is heresy and shall be ignored when possible.
    int place = x + (y << 6);
    char8_t picked = pick(place);
    return picked;
}

//////////////////////////////////////
//CLASSIFICATION RAW MAP MANIPULATION
//////////////////////////////////////

/**
 * <h1>Function COPY_MAP</h1>
 * A function built to return a raw operable array of HEIGHTs. This is still encoded into its primal form, compacted, with each char representing 4 tiles at once.
 * @return a char array copied from the instantaneous value present of this object's MAP
 */
std::array<char8_t, 1024> MAP::copy_map() {
    std::array<char8_t, 1024> copy = {};
    for (int i = 0; i < size; i++){
        copy[i] = map[i];
    }
    return copy;
}

/**
 * <h1>Function SET_HEIGHTMAP</h1>
 * Overwrites the entire heightmap array
 * @param array The new array to write onto this object's heightmap
 */
void MAP::set_heightmap(std::array<char8_t, 1024> new_map){
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
std::array<char8_t, 4> MAP::uncompress(char8_t c){
    std::array<char8_t, 4> out = {};
    for (int i = 0; i < 4; i++){
        out[i] = (char8_t) (c & 3);
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
char8_t MAP::compress(std::array<char8_t, 4> in){
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
char8_t MAP::pick(int place) {
    place = std::clamp(place, 0, 4095);
    unsigned int cSpace = place >> 2;
    char8_t cBit = map[cSpace];
    std::array<char8_t, 4> a = uncompress(cBit);
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
void MAP::pack(int place, char8_t value) {
    place = std::clamp(place, 0, 4095);
    unsigned int cSpace = place >> 2;
    char8_t cBit = map[cSpace];
    std::array<char8_t, 4> a = uncompress(cBit);
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
void MAP::dump_map(){
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
void MAP::out(){
    // Outputting 4096 values in a 64x64 array
    int place = 0;
    int line = 0;
    for (int y = 0; y < 64; ++y)
    {
        std::cout << std::endl << "#" << line << ":  \t" ;
        line++;
        for (int x = 0; x < 64; ++x)
        {
            char8_t h = pick(place);
            place++;
            switch(h){
                case 0b00000000:
                    std::cout << "░░";
                    break;
                case 0b00000001:
                    std::cout << "▒▒";
                    break;
                case 0b00000010:
                    std::cout << "▓▓";
                    break;
                default:
                    std::cout << "██";
            }
        }

    }
}

void MAP::set_seed(std::string val) {
    this->seed = std::move(val);
}

void MAP::set_vBias(float val) {
    const float t = val < -4.0f ? -4.0f : val;
    this->vBias =  t > 4.0f ? 4.0f : t;
}

void MAP::set_scalar(float val) {
    const float t = val < 0.00001f ? 0.00001f : val;
    this->scalar = t > 4.0f ? 4.0f : t;
}

void MAP::set_roughness(float val) {
    const float t = val < 0.0f ? 0.0f : val;
    this->roughness = t > 2.0f ? 2.0f : t;
}

void MAP::set_w0(float val) {
    const float t = val < 1.0f ? 1.0f : val;
    this->w0 = t > 64.0f ? 64.0f : t;
}

void MAP::set_w1(float val) {
    const float t = val < 1.0f ? 1.0f : val;
    this->w1 = t > 64.0f ? 64.0f : t;
}

void MAP::set_w2(float val) {
    const float t = val < 1.0f ? 1.0f : val;
    this->w2 = t > 64.0f ? 64.0f : t;
}

void MAP::set_w3(float val) {
    const float t = val < 1.0f ? 1.0f : val;
    this->w3 = t > 64.0f ? 64.0f : t;
}


std::string MAP::get_seed() const {
    return seed;
}

float MAP::get_vBias() const {
    return vBias;
}

float MAP::get_scalar() const{
    return scalar;
}

float MAP::get_roughness() const {
    return roughness;
}

float MAP::get_w0() const {
    return w0;
}

float MAP::get_w1() const {
    return w1;
}

float MAP::get_w2() const {
    return w2;
}

float MAP::get_w3() const {
    return w3;
}

void MAP::generate() {
    const siv::PerlinNoise::seed_type lseed = *(int*) (seed.c_str());
    const siv::PerlinNoise perlin{ lseed };

    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            const double noise = (perlin.octave2D_01((x * scalar), (y * scalar), 2)*4) + vBias;
            set((char8_t) noise, x, y);
        }
    }
}










