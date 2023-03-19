//
// Created by Triage on 2/10/2023.
//

#ifndef HALCYONICUS_REGION_H
#define HALCYONICUS_REGION_H

#include <filesystem>
#include "world.h"
#include "regioncoord.h"

/**
 * Responsible for holding many bits of the world and all of their data, so that all of it can be saved on disk and read from freely
 *
 */
class REGION{

public:
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
     *  From byte 32 to 1055 listed as hashes for each chunk
     *  from byte 1056 to 2079 reserved for timestamps of all chunks
     *  from byte 2080 to onward, for a repeat of 15360 per next iteration, in order list the chunks at starting array place of 0 to final array place of 255
    */

    /**
     * Writes to disk the world at this area, to a region file present on disk. If the region is not present, it will
     * create another region file and write it to that.
     * <br>
     * A way to save a singular world chunk. If you expect to save many at once, consider using saveChunks() instead.
     * Uses a scanner queue to dynamically dispatch workers to handle world saving on an interval async
     * @param world The world tile to save
     */
    static bool writeChunk(WORLD *world);

    /**
     * Reads a single world shard. uses a best-effort reader method to access disk data as fast as possible. will block
     * until finished with the operation
     * @param worldcoord The worldcoord to read
     * @return The world if any that occupies that worldcoord
     */
    static bool readChunk(WORLD *world);

    /**
     * Checks if a partition of the world is loaded or not on disk
     * @param worldcoord The worldcoord to check
     * @return true if the area exists, false if not
     */
    static bool worldExists(WORLDCOORD worldcoord);


    ///////////////////////////

    /**
     * Tests to see if a region exists by parsing the region file name from the coordinate and using isOpen() to discern if it exists
     * @param regioncoord The coordinate to test for
     * @return If it exists on disk or not
     */
    static bool regionExists(REGIONCOORD regioncoord);

    /**
     * Finds the filename without directory context of a region. Sample output may look like r_0_0.hcr from Region 0,0
     * @param regioncoord The region to parse
     * @return The output string as a filename
     */
    static std::string parseRegioncoordToFname(REGIONCOORD regioncoord);

    /**
     * Converts a filename to a region coordinate. Automatically throws an error if there are too many places or numbers exceed the max integer.
     * @param fname the file name in string format
     * @return the region coordinate extracted
     */
    static REGIONCOORD parseFnameToRegioncoord(const std::string& fname);

    /**
     * Derives the region coordinate from a world passed in
     * @param world The world shard pointer
     * @return the region the world shard is in
     */
    static REGIONCOORD findRegioncoordFromWorldShard(WORLD *world);

    /**
     * A useful tool to figure out where a worldshard is within its region array. The region file is simply an array of world shards.
     * @param chunkLocation The worldcoord of the chunk that we want to find the offset to
     * @return The array coordinate that this worldcoord occupies
     */
    static int findChunkArrayOffset(WORLDCOORD chunkLocation);

    static void setChunkExists(int arrayOffset, std::fstream* fstream);

    //The function returns an int to show if it exists or not, so we can use this as the value returned
    static char chunkExists(int arrayOffset, std::fstream *fstream);

    static int getHash(int arrayOffset, std::fstream *fstream);

    static void setHash(int arrayOffset, std::fstream *fstream, int data);

    static int getTimestamp(int arrayOffset, std::fstream *fstream);

    static void setTimestamp(int arrayOffset, std::fstream *fstream, int timestamp);

    static void readWorldData(int arrayOffset, std::fstream *fstream, WORLD *world);

    static void writeWorldData(int arrayOffset, std::fstream *fstream, WORLD *world);

    static std::string prependWorldDir(const std::string& in);

    static bool createDirectories();

    static std::filesystem::path parseFullPathFromRegionCoord(REGIONCOORD regioncoord);

    static void createEmptyWorld(const std::filesystem::path& path);

    static bool checkForDirectoryStructure();
};

#endif //HALCYONICUS_REGION_H
