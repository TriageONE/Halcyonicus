//
// Created by Triage on 5/25/2023.
//

#ifndef HALCYONICUS_CHUNK_H
#define HALCYONICUS_CHUNK_H

#ifndef HAL_WORLD_HEIGHT
#define MAX_WORLD_HEIGHT 256
#define MIN_WORLD_HEIGHT 0
#endif

#include <map>
#include <mutex>
#include <cstring>
#include <algorithm>
#include <set>
#include <sstream>

/**
 * A rapid scope change has occured, and i now plan to exact a change to how we plan the world. No longer would a world be flat, but now fully vertical and in first person
 */

class CHUNK{

public:

    struct CHUNK_COORD{
        long x;
        long y;
    };

    //Holds a relative coordinate within the chunk, with the target as a single block
    struct BLOCK_RELATIVE{
        char x;
        char y;
        short z;
    };

private:
    /**
     * Each column should contain a vector of blocks and their offsets, defined by a map of shorts to strings.
     */
    struct CHUNK_COLUMN{
        //unsigned char x;    //Defines the relative location of the column on the X plane, East and west, where 0 is west and 31 is east
        //unsigned char y;    //Defines the relative location of the column on the Y plane, up and down, where 0 is north and 31 is south
        //These can be implicitly defined. We will always need 32x32 columns, therefore we would need an array of columns present, rather than a vector holding columns
        std::mutex mapMutex;
        std::map< short, short > blocks; //defines the mapping of blocks and their IDs
    } columns[32][32];

    std::set<short> locations;

    /**
     * Represents the location of the chunk in the world, and can translate to a region coordinate
     */
    CHUNK_COORD current_coord;

public:

    CHUNK(long x, long y){
        this->current_coord.x = x;
        this->current_coord.y = y;
    }

    /**
     * Sets the type at the location of that chunk to a new type, specified by an integer to describe its type
     * @param x the east-west coordinate, relative to the chunk clamped between 0-31
     * @param y the north-south coordinate, relative to the chunk clamped between 0-31
     * @param z the height component that ranges from 0-256, or in some cases more depending on how it may be set here
     * @param type the type that should be present at that location
     */
    void setBlockRelative(char x, char y, short z, int type){
        char cx = std::clamp((int) x, 0, 31) , cy = std::clamp((int) y, 0, 31) ;
        CHUNK_COLUMN * currentColumn = &columns[cx][cy];

        currentColumn->mapMutex.lock();
        currentColumn->blocks.erase(z);
        currentColumn->blocks.insert({z, type});
        currentColumn->mapMutex.unlock();
    }

    /**
     * Checks the chunk for if there is a block at that location. Displays false for air blocks
     * @param x the east-west coordinate, relative to the chunk clamped between 0-31
     * @param y the north-south coordinate, relative to the chunk clamped between 0-31
     * @param z the height component that ranges from 0-256, or in some cases more depending on how it may be set here
     */
    bool blockExistsRelative(char x, char y, short z){
        char cx = std::clamp((int) x, 0, 31) , cy = std::clamp((int) y, 0, 31) ;
        CHUNK_COLUMN * currentColumn = &columns[cx][cy];

        currentColumn->mapMutex.lock();
        bool c = currentColumn->blocks.contains(z);
        currentColumn->mapMutex.unlock();
        return c;
    }

    /**
     * Returns the type given at the location specified
     * @param x the east-west coordinate, relative to the chunk clamped between 0-31
     * @param y the north-south coordinate, relative to the chunk clamped between 0-31
     * @param z the height component that ranges from 0-256, or in some cases more depending on how it may be set here
     * @return the type present at that location
     */
    short getBlockRelative(char x, char y, short z){
        char cx = std::clamp((int) x, 0, 31) , cy = std::clamp((int) y, 0, 31) ;
        CHUNK_COLUMN * currentColumn = &columns[cx][cy];
        int result;
        currentColumn->mapMutex.lock();
        auto it = currentColumn->blocks.find(z);
        if (it == currentColumn->blocks.end()) result = 0;
        else result = it->second;
        currentColumn->mapMutex.unlock();
        return result;
    }

    /**
     * Given a set of real world coordinates, find the relative coordinates within its chunk
     * @param x the east-west coordinate
     * @param y the north-south coordinate
     * @param z the height component that ranges from 0-256, or in some cases more depending on how it may be set here
     * @return the set of relative coordinates for a block in a chunk
     */
    static BLOCK_RELATIVE findRelativeCoordinatesFromAbsolute(long long x, long long y, short z){
        return {(char) (x & 31), (char) (y & 31), z };
    }

    void setColumn(char x, char y, std::map< short, short > * newColumn){
        char cx = std::clamp((int) x, 0, 31) , cy = std::clamp((int) y, 0, 31) ;
        CHUNK_COLUMN * currentColumn = &columns[cx][cy];
        currentColumn->blocks = *newColumn;
    }

    /*
     * Turn the column into a string to be stored later
     */
    std::string serializeColumn(char x, char y){
        std::stringstream ss;
        //we should keep track of all our types within the column
        std::map<int, std::set<short>> types;
        CHUNK_COLUMN * currentColumn = &columns[x][y];
        ss << "{ ";
        currentColumn->mapMutex.lock();
        for (auto pair : currentColumn->blocks){
            auto it = types.find(pair.second);
            if (it == types.end()){
                std::pair<int, std::set<short>> newPair {pair.second, {}};
                newPair.second.insert(pair.first);
                types.insert(newPair);
            } else {
                it->second.insert(pair.first);
            }
        }
        //We are all sorted, now we can focus on building our string
        for (auto pair : types){
            for (auto i : pair.second){
                ss << i << ' ';
            }
            ss << "# ";
        }
        ss << '}';

        currentColumn->mapMutex.unlock();
        return ss.str();
    }

    /*
     * Turn a string that was once a column back into a column, within this chunk. Returns true if it worked
     */
    bool deserializeColumn(char x, char y, std::string data){
        long long length = data.length();
        if (length <= 3){
            //empty chunk
            return false;
        }
        std::stringstream iss(data);
        std::string token;
        int type = -1;
        short location;
        std::map< short, short > tempBlocks;
        bool hasType = false, hasStarted = false;

        while (iss >> token) {
            if (token == "{"){
                hasStarted = true;
            }

            if (!hasStarted){
                return false; //Error state where we found the first token as something other than an opening brace
            }

            if (token.length() > 11 || token.length() < 1){
                return false; //Error state where we exceeded the maximum integer by an order of an entire place
            }

            if (!hasType){
                //If we dont have a type yet, assign this token to a type but first make sure that it's a valid input
                if (!isNumericString(token)){
                    continue;
                }
                type = std::stoi(token);
                hasType = true;
                continue;
            }

            if (token == "#" ){
                hasType = false;
                continue;
            }

            if (token == "}" ){
                break;
            }

            if (!isNumericString(token)){
                continue;
            }
            int temp = std::stoi(token);
            location = (short) std::clamp(temp, MIN_WORLD_HEIGHT, MAX_WORLD_HEIGHT);
            tempBlocks.insert({location, type});

        }
        setColumn(x, y, &tempBlocks);
        return true;
    }



private:

    bool isNumericString(const std::string& str) {
        for (char c : str) {
            if (!std::isdigit(c) && c != '-') {
                return false;
            }
        }
        return true;
    }

    short getPackedRelativeLocation(char x, char y){
        short id;
        id = (short) x << 8;
        id |= (short) y;
        return id;
    }

};

#endif //HALCYONICUS_CHUNK_H
