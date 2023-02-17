//
// Created by Triage on 2/10/2023.
//

#ifndef HALCYONICUS_REGION_H
#define HALCYONICUS_REGION_H

#include "world.h"

/**
 * Responsible for holding many bits of the world and all of their data, so that all of it can be saved on disk and read from freely
 *
 */
class REGION{
    struct REGIONCOORD{
        int x, y;
        REGIONCOORD(int x, int y){
            this->x = x;
            this->y = y;
        }
    };
    /**
     * The region object should have the ability to load and unload 16x16 plot of world chunks in a single file.
     * ideally, we have all the chunks loaded but this is not always possible. Almost all of the time, players will only
     * load one chunk at a time and load only that chunk, not the entire region at once.
     *
     * a new player spawns. They spawn at 0,0, and request the surrounding 3x3 chunks. This will be needed to be loaded
     * from disk. The server already generated those areas, and somehow (not yet figured out) placed them on the disk.
     * Only a fraction of the loaded chunks are called for.
     *
     * lets say we start a brand new server and we need to make spawn. upon startup, we go through the following stages:
     * startup -> request to generate<areas> -> generation -> save those areas.
     *
     * In order to save any area, we have to consider the way that the actual chunk is torn apart and saved. Delimitation
     * is important if we plan to have a single file with a chunk in it. There must be a way to store chunks that exist
     * as singlets and may be disjointed from other chunks. There should also be a way to find an individual chunk without
     * loading any more than we would need, such as when a player loads a single chunk from the world without the rest of
     * the region
     */

    //This as a full concept should be a static class used for getting world shards, rather than a class that may hold world shards
    //Therefore we should not have any class variables and just have static function members.
    //The language of this class should be "Summon this class to interact with the filesystem of worlds"

    /**
     * REGION FILE STRUCTURE:
     * - We need to know what is generated and what is not generated, and we can represent this with a 256 bit field, since
     *      its either 1 or zero.
     * - for each generated chunk, we must know each timestamp. a timestamp can be a unix timestamp in seconds. This must
     *      be 32 bits.
     * - Each chunk should have a random hash worth 1 byte per, to account for a special hash that is entirely random and
     *      unpredictable, where this hash changes every time the world is actually saved. This may be used for raft consensus
     *      later.
     * - Each chunk is exactly 17 physical layers, 1 climate layer, and 1 saturation map. this makes exactly 19 layers, and
     *      therefore exactly 19 maps. at 19 maps, each layer is 1024 bytes, therefore each map is exactly 19456 bytes.
     *      if we store all the maps, we make a 6MB file, out of maps alone. This can be compressed of course.
     */

    /** DEPRECATED:
     * First 32 bytes dedicated to if a chunk is loaded or not  START 0 END 31
     * next 1024 bytes for timestamps, 4 bytes X 256 shards     START 32 END 1055
     * next 256 bytes dedicated to the hashes for each chunk    START 1056 end 1311
     * after this, we start with the array of shards.           START 1312 EVERY (15360)..*256
     *      each shard is 15360 bytes long
     *      there will be 256 of these areas
     */

    /** CURRENT:
     * REVISED DATA PACKING:
     *  From 0 to 31 bytes reserved for isExists() of chunks,
     *  From byte 32 to 287 listed as hashes for each chunk
     *  from byte 288 to 1311 reserved for timestamps of all chunks
     *  from byte 1312 to onward, for a repeat of 15360 per next iteration, in order list the chunks at starting array place of 0 to final array place of 255
    */

    /**
     * Writes to disk the world at this area, to a region file present on disk. If the region is not present, it will
     * create another region file and write it to that.
     * <br>
     * A way to save a singular world chunk. If you expect to save many at once, consider using saveChunks() instead.
     * Uses a scanner queue to dynamically dispatch workers to handle world saving on an interval async
     * @param world The world tile to save
     */
    static bool writeChunk(WORLD world);

    /**
     * Reads a single world shard. uses a best-effort reader method to access disk data as fast as possible. will block
     * until finished with the operation
     * @param worldcoord The worldcoord to read
     * @return The world if any that occupies that worldcoord
     */
    static WORLD readChunk(WORLD::WORLDCOORD worldcoord);

    /**
     * Checks if a partition of the world is loaded or not on disk
     * @param worldcoord The worldcoord to check
     * @return true if the area exists, false if not
     */
    static bool worldExists(WORLD::WORLDCOORD worldcoord);

    /**
     * Writes to disk the world at this area, to a region file present on disk. If the region is not present, it will
     * create another region file and write it to that.
     * Adds all included world shards to a scanner queue to dynamically dispatch workers to handle world saving on an interval async
     * @param worlds
     */
    static void writeChunks(std::vector<WORLD> worlds);

    /**
     * Reads many chunks at once and returns a vector of them all. Opportunistically reads as fast as possible and blocks
     * until all chunks are returned.
     * @param worldcoords
     * @return
     */
    static std::vector<WORLD> readChunks(std::vector<WORLD::WORLDCOORD> worldcoords);

    ///////////////////////////

    static bool regionExists(REGIONCOORD regioncoord);
    static std::string parseRegioncoordToFname(REGION::REGIONCOORD regioncoord);
    static REGION::REGIONCOORD parseFnameToRegioncoord(const std::string& fname);
    static REGION::REGIONCOORD findRegioncoordFromWorldShard(WORLD *world);
    static int findChunkArrayOffset(LOCATION chunkLocation);

    static void setChunkExists(int arrayOffset, std::fstream* fstream);
    static bool chunkExists(int arrayOffset, std::fstream *fstream);


    char8_t getHash(int arrayOffset, std::fstream *fstream);

    char8_t setHash(int arrayOffset, std::fstream *fstream, char data);

    int getTimestamp(int arrayOffset, std::fstream *fstream);

    void setTimestamp(int arrayOffset, std::fstream *fstream, int timestamp);

    void getWorldData(int arrayOffset, std::fstream *fstream, WORLD *world);

    void setWorldData(int arrayOffset, std::fstream *fstream, WORLD *world);
};

#endif //HALCYONICUS_REGION_H
