//
// Created by Aron Mantyla on 6/16/23.
//

#ifndef HALCYONICUS_CHUNK2_H
#define HALCYONICUS_CHUNK2_H

#include <string>
#include "worldcoord.h"
#include "stdio.h"
/**
 * Defines the new chunk format for representing individual columns of heights with no concavity
 *
 * array 'heights' returns a height for that location. Find the array position with getRelativeArrayPosition()
 * array 'data' defines the top type, like grass or snow, the depth of that topper, and the starting point of the tough stone
 *  Data is packed with the first nibble being the top type ranging from 0-15, then the next nibble being the top depth from 0-15. The final byte is the starting point of the tough stone
 */
class CHUNK2 {
public:
    //Requires an array of heights present, organized by array location
    unsigned char heights[256]{0};

    /*For the column, describe the data with respect to
     - top panel and its depth,
     - when the stone layer starts

     stone layer start should be a char
     top panel requires a type and a depth, there should be 16 types of top panels and a max depth of 16 therefore a char

     Entire thing can be packed in a short

     0x XX    XX
        Top panel type
         Top panel depth
              stone layer start

     */

    short data[256]{0};

    //Must identify itself with a location in the world, as a chunk location
    WORLDCOORD location;

    explicit CHUNK2(WORLDCOORD location){
        this->location = location;
    };

    //Setheight and setdata are not needed, we can manipulate the array from here
    //Instead find an easy way to find indicies of relative locations
    static unsigned char getRelativeArrayPosition(unsigned char x, unsigned char z) {
        return (x * 16) + z;
    }

    struct relCoords { char x, z; };
    static relCoords getRelativeCoordinates(unsigned char pos){
        return { static_cast<char>(pos / 16), static_cast<char>(pos % 16)};
    }

    void setTopType(char xRel, char zRel, char newType){
        //clamp rels to 0-15
        xRel = std::clamp((int)xRel, 0, 15);
        zRel = std::clamp((int)zRel, 0, 15);
        newType = std::clamp((int)newType, 0, 15) << 12;

        auto pos = getRelativeArrayPosition(xRel, zRel);

        short newData = data[pos] & 0x0FFF;
        data[pos] = newData | (( (short) newType ) << 12);

    };

    void setTopDepth(char xRel, char zRel, char newDepth){
        //clamp rels to 0-15
        xRel = std::clamp((int)xRel, 0, 15);
        zRel = std::clamp((int)zRel, 0, 15);

        newDepth = (short)std::clamp((int)newDepth, 0, 15) << 8;

        auto pos = getRelativeArrayPosition(xRel, zRel);

        short newData = data[pos] &  0xF0FF;
        data[pos] = newData | (( (short) newDepth ) << 8);
    };

    void setStartOfToughMaterial(char xRel, char zRel, unsigned char newStart){
        //clamp rels to 0-15
        xRel = std::clamp((int)xRel, 0, 15);
        zRel = std::clamp((int)zRel, 0, 15);

        auto pos = getRelativeArrayPosition(xRel, zRel);
        newStart = (short)std::clamp((int)newStart, 0, (int) heights[pos]);
        short newData = data[pos] &  0xFF00;
        data[pos] = newData | ( (short) newStart );
    };

    unsigned char getStartOfToughMaterial(short data){
        return (unsigned char)(data & 0x00FF);
    }

    unsigned char getTopType(short data){
        return (unsigned char)((data & 0xF000) >> 12);
    }

    unsigned char getTopDepth(short data){
        return (unsigned char)((data & 0x0F00) >> 8);
    }


    unsigned char getStartOfToughMaterial(char position){
        return (unsigned char)(data[position] & 0x00FF);
    }

    unsigned char getTopType(char position){
        return (unsigned char)((data[position]  & 0xF000) >> 12);
    }

    unsigned char getTopDepth(char position){
        return (unsigned char)((data[position]  & 0x0F00) >> 8);
    }


    unsigned char getStartOfToughMaterial(char relX, char relZ){
        short dat = getRelativeArrayPosition(relX, relZ);
        return (unsigned char)(dat & 0x00FF);
    }

    unsigned char getTopType(char relX, char relZ){
        short dat = getRelativeArrayPosition(relX, relZ);
        return (unsigned char)((dat & 0xF000) >> 12);
    }

    unsigned char getTopDepth(char relX, char relZ){
        short dat = getRelativeArrayPosition(relX, relZ);
        return (unsigned char)((dat & 0x0F00) >> 8);
    }


    //Serializers/Deserializers

    std::vector<char> serialize(){
        std::vector<char> out;
        for(short i = 0; i <= 255; i++){
            out.push_back(heights[i]);
            short tmp = data[i];

            char tmp2[2] {0};
            ::memcpy(tmp2, &tmp, 2);

            out.push_back(tmp2[0]);
            out.push_back(tmp2[1]);
        }
        return out;
    }

    void deserializeIntoThis(std::vector<char> in){
        short j = 0;
        for(short i = 0; i <= 255; i++){
            this->heights[i] = in[j];
            j++;
            char tmp[2] {0};
            tmp[0] = in[j];
            j++;
            tmp[1] = in[j];
            j++;
            data[i] = *(short *) tmp;
        }
    }


};

#endif //HALCYONICUS_CHUNK2_H
