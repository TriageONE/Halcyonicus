//
// Created by Triage on 8/28/2023.
//

#ifndef HALCYONICUS_CHUNK_H
#define HALCYONICUS_CHUNK_H

//#include "../logging/hlogger.h"
//#include "../world/block.h"

using namespace hlogger;

class CHUNK{
public:
    class LAYER {
    public:
        // Denotes the level under the ground this layer is where 0
        //  is the topmost layer of the world and 255 is the deepest
        unsigned char level = 0;

        // A list of heights for this layer
        short heights[64][64] {0};

        /* A byte-packed descriptor for materials that exist for each column
         *
         * interpreted as char array[4],
         *  place 0 is the top state
         *  place 1 is the type of column material
         *  place 2-3 is a short prim for the stopping point of hard stone
         */
        int descriptor[64][64] {0};
        bool changed = false;

        LAYER() = default;

        explicit LAYER(unsigned char level) : level{level} {}

        void serializeLayer(std::vector<unsigned char> * dst) {
            dst->clear();
            if (dst->max_size() < 20480)
                dst->resize(20480);
            char height[2];
            for (short x = 0; x <= 63; x++) {
                for (short y = 0; y <= 63; y++) {
                    ::memcpy(height,&heights[x][y], 2);
                    dst->push_back(height[0]);
                    dst->push_back(height[1]);
                }
            }
            char desc[4];
            for (short x = 0; x <= 63; x++) {
                for (short y = 0; y <= 63; y++) {
                    ::memcpy(desc,&descriptor[x][y], 4);
                    dst->push_back(desc[0]);
                    dst->push_back(desc[1]);
                    dst->push_back(desc[2]);
                    dst->push_back(desc[3]);
                }
            }
        }

        void deserializeLayer(std::vector<unsigned char> * src){
            if (src->max_size() < 20480) {
                //err << "Deserializing terrain data was less than 20480 bytes, got " << src->size() << " instead, skipping this operation. Terrain for level " << level << " was untouched." << nl;
                return;
            }
            unsigned char height[2];
            for (short x = 0; x <= 63; x++){
                for (short y = 0; y <= 63; y++){
                    height[0] = src->at(((x * 63) + y) * 2);
                    height[1] = src->at((((x * 63) + y) * 2) + 1);
                    this->heights[x][y] = *(short*) height;
                }
            }
            unsigned char desc[4];
            for (short x = 0; x <= 63; x++){
                for (short y = 0; y <= 63; y++){
                    desc[0] = src->at((((x * 63) + y) * 4) + 8193);
                    desc[1] = src->at((((x * 63) + y) * 4) + 8194);
                    desc[2] = src->at((((x * 63) + y) * 4) + 8195);
                    desc[3] = src->at((((x * 63) + y) * 4) + 8196);
                    this->descriptor[x][y] = *(int*) desc;
                }
            }
        }
    };

    // Its offset denoting location within the region
    COORDINATE::WORLDCOORD location;

    // Maps denoting the general temperature values from the chunk
    unsigned char   temperature[16][16] {0},
                    humidity[16][16] {0};

    // Defaultly set to false since we should specify when anything has changed to preserve database writes and reads
    bool changed = false;
    bool existsPersistent = true;

    LAYER layers[24];

    std::vector<BLOCK> blocks;

    CHUNK() {};

    explicit CHUNK(COORDINATE::WORLDCOORD location) : location{location} {}

    void serializeClimate(std::vector<unsigned char> * toEmplaceInto) {
        toEmplaceInto->clear();
        if (toEmplaceInto->max_size() < 512)
            toEmplaceInto->resize(512);

        for (short x = 0; x <= 15; x++){
            for (short y = 0; y <= 15; y++){
                toEmplaceInto->push_back(this->temperature[x][y]);
            }
        }
        for (short x = 0; x <= 15; x++){
            for (short y = 0; y <= 15; y++){
                toEmplaceInto->push_back(this->humidity[x][y]);
            }
        }
    }

    void deserializeClimate(std::vector<unsigned char> * toReadFrom) {
        if (toReadFrom->size() < 512){
            //err << "Deserializing climate data was less than 512 bytes, got " << toReadFrom->size() << " instead, skipping this operation. Climate for " << location.x << "x, " << location.y << "y was untouched." << nl;
            return;
        }
        for (short x = 0; x <= 15; x++){
            for (short y = 0; y <= 15; y++){
                this->temperature[x][y] = toReadFrom->at(((x * 16) + y));
            }
        }
        for (short x = 0; x <= 15; x++){
            for (short y = 0; y <= 15; y++){
                this->humidity[x][y] = toReadFrom->at(((x * 16) + y) + 256);
            }
        }
    }

};

#endif //HALCYONICUS_CHUNK_H
