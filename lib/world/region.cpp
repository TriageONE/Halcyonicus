//
// Created by Triage on 2/10/2023.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include "region.h"
#include "regioncoord.h"

bool REGION::writeChunk(WORLD* world) {
    //Should be preceded by check if region exists

    //Must read entire region in order to implement changes to chunks
    std::fstream regionFile;
    //find the name of the file
    REGIONCOORD regioncoord = findRegioncoordFromWorldShard(world);
    //Parse it to a filename
    std::stringstream filename;
    filename << "./world/" << parseRegioncoordToFname(regioncoord);

    int arrayOffset = findChunkArrayOffset(world->getLocation());

    regionFile.open(filename.str());
    if (!(regionFile)) return false;

    //We just attempted to open it. If its open, we can start writing.
    if(regionFile.is_open()){
        //We have exactly one world that we want to write to the region.
        // all we have to do is update the existence table, timestamp and hash.
        // We could write to the existence table multiple times but its more efficient to read and then write if needed

        char exists = chunkExists(arrayOffset, &regionFile);
        if (!exists){
            setChunkExists(arrayOffset, &regionFile);
        }
        //Check if there are no changes to be made
        int hash = world->getHash();
        int hash2 = getHash(arrayOffset, &regionFile);

        //If we find that the worlds are the exact same, we can assume that we should not do anything more and simply leave the area alone.
        if (hash == hash2) return true;

        //Write the world data
        writeWorldData(arrayOffset, &regionFile, world);

        //We will always want to update the timestamp, but this should likely occur after the world write
        int timestamp = (int) std::time(0);
        setTimestamp(arrayOffset, &regionFile, timestamp);
    } else return false;
    return true;
}

bool REGION::readChunk(WORLD* world) {
    //Affect the linked world to be overwritten with new data from the file
    //Must read entire region in order to implement changes to chunks
    std::fstream regionFile;
    //find the name of the file
    REGIONCOORD regioncoord = findRegioncoordFromWorldShard(world);
    //Parse it to a filename
    std::stringstream filename;
    filename << "./world/" << parseRegioncoordToFname(regioncoord);

    int arrayOffset = findChunkArrayOffset(world->getLocation());

    regionFile.open(filename.str());
    if (!(regionFile)) return false;
    //We just attempted to open it. If its open, we can start writing.
    if(regionFile.is_open()){
        //We have exactly one world that we want to Read from the region.
        //Make sure it exists and go for it

        char exists = chunkExists(arrayOffset, &regionFile);
        if (!exists){
            return false;
        }
        //Read the world data
        readWorldData(arrayOffset, &regionFile, world);
    } else return false;
    return true;

}

bool REGION::worldExists(WORLDCOORD worldcoord) {
    //A region coordinate indicates an entire region, however a worldcoord indicates a region within that, and that was represented by an existence table
    //Find the region file first
    std::fstream regionFile;
    REGIONCOORD regioncoord = worldcoord.getRegionCoordinates();
    //Parse it to a filename
    std::stringstream filename;
    filename << "./world/" << parseRegioncoordToFname(regioncoord);

    int arrayOffset = findChunkArrayOffset(worldcoord);

    regionFile.open(filename.str());
    if (!(regionFile)) return false;

    return chunkExists(arrayOffset, &regionFile);

}


bool REGION::regionExists(REGIONCOORD regioncoord) {
    std::ifstream regionFile;
    std::stringstream name;
    name << "rg_" << std::to_string(regioncoord.getX()) << "_" << std::to_string(regioncoord.getY()) << ".hcr";
    regionFile.open(name.str());
    return regionFile.is_open();
}

////////////////////////////////////
/// File interpretation and parsing

std::string REGION::parseRegioncoordToFname(REGIONCOORD regioncoord) {
    std::stringstream name;
    name << "rg_" << std::to_string(regioncoord.getX()) << "_" << std::to_string(regioncoord.getY()) << ".hcr";
    return name.str();
}

REGIONCOORD REGION::parseFnameToRegioncoord(const std::string& fname) {
    int x, y;
    try{
        if (!(fname[0] == 'r' && fname[1] == 'g' && fname[2] == '_')){
            throw std::domain_error(R"(Does not match the required named convention, must start with "rg_"!)");
        }
        std::stringstream ss;
        const std::string comp = "0123456789-";
        short tr = 4;
        for (int i = 3; ;i++){
            if (comp.contains(fname[i])) ss << fname[i];
            if (fname[i] == '_') break;
            if (i > 14) throw std::out_of_range(R"(First coordinate X does not match the required named convention, must not exceed millions place!)");
            tr++;
        }
        x = std::stoi(ss.str());
        ss.clear();
        for (;;tr++){
            if (comp.contains(fname[tr])) ss << fname[tr];
            if (fname[tr] == '.') break;
            if (tr > 26) throw std::out_of_range(R"(Second coordinate Y does not match the required named convention, must not exceed millions place!)");
        }
        y = std::stoi(ss.str());
        ss.clear();
    } catch (std::exception &e) {
        std::cerr << "FILEPARSE CONVENTION ERROR: " << e.what() << std::endl;
        return {0,0};
    }
    return {x, y};
}

REGIONCOORD REGION::findRegioncoordFromWorldShard(WORLD *world) {
    WORLDCOORD l = world->getLocation();
    return {l.getX() >> 4, l.getZ() >> 4};
}

int REGION::findChunkArrayOffset(WORLDCOORD chunkLocation) {
    /*
     * When preparing the chunk array offset, we only need to know the chunk location because we can assume array positioning
     * based on the x and z of the location
     * a chunk at -1 -1 exists in region -1 -1. its on the top row of the array, place 15
     * a chunk at -16 -16 exists in region -1 -1 still, Its the 240th place right?
     * a chunk at -1 -16 would be at place 255 because there is no array object 256
     */

    int x = chunkLocation.getX();
    int y = chunkLocation.getZ();

    int xoff = x & 15;
    int yoff = y & 15;

    return (yoff * 16) + xoff;
}

//////////////////////////////////
/// Database tools

void REGION::setChunkExists(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    //Save the last points, since there may have been an interaction that might've taken place before this requiring a placement
    std::streampos lastGetPoint = fstream->tellg();
    std::streampos lastPutPoint = fstream->tellp();

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data;

    unsigned int writePoint = (arrayOffset / 8);
    fstream->seekg( writePoint ); // If we just divide the array position by the length of the data, we can get exactly the offset needed to read, plus 32 for the start of the data
    fstream->seekp( writePoint ); // Same for put position, we will be writing to the same spot
    fstream->read(&data, 1);

    //we have the byte, now we mask it and OR it with a special value
    data = data | (1 << (arrayOffset % 8));

    //Write the data to the file
    fstream->write(&data, 1);

    //restore the seek points to the original point
    fstream->seekg( lastGetPoint );
    fstream->seekp( lastPutPoint );
    //This does not close the stream, as closing the file would finalize the file to disk

}

char REGION::chunkExists(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    std::streampos lastGetPoint = fstream->tellg();

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data;

    unsigned int readPoint = (arrayOffset / 8);

    fstream->seekg( readPoint ); // If we just divide the array position by the length of the data, we can get exactly the offset needed to read, plus 32 for the start of the data
    fstream->read(&data, 1);
    fstream->seekg( lastGetPoint );

    // Should read as:
    //    If the bit at the position is a 1, then return true. Evaluation to anything other than 0 is true.
    return (data & (1 << (arrayOffset % 8)));
}

//////////////////

int REGION::getHash(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    std::streampos lastGetPoint = fstream->tellg();

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data[4];

    fstream->seekg( (arrayOffset * 4) + 32 );
    fstream->read(data, 4);
    fstream->seekg( lastGetPoint );

    // Should read as:
    //    If the bit at the position is a 1, then return true. Evaluation to anything other than 0 is true.
    return *(int*) data;
}

void REGION::setHash(int arrayOffset, std::fstream *fstream, int data) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    std::streampos lastPutPoint = fstream->tellp();

    char d2[4];
    ::memcpy(d2, &data, 4);

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    fstream->seekp( (arrayOffset * 4) + 32 );
    fstream->write(d2, 4);
    fstream->seekp(lastPutPoint );
}

//////////

int REGION::getTimestamp(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    char data[4] {};

    std::streampos lastGetPoint = fstream->tellg();

    fstream->seekg( (arrayOffset * 4) + 1056 );
    fstream->read(data, 4);
    fstream->seekg( lastGetPoint );

    return *(int*) data; //Dangerous typecast to integer. Maybe this wont work? i guess if we do this the exact same way on write then it doesnt matter right
    // I tested in a scratch file and it seems that this wants to work. Only thing we need to be certain of is the data length.

}

void REGION::setTimestamp(int arrayOffset, std::fstream *fstream, int timestamp) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);
    char data[4] {};
    ::memcpy(data,&timestamp,4);

    std::streampos lastPutPoint = fstream->tellp();

    fstream->seekp( (arrayOffset * 4) + 1056 );
    fstream->write(data, 4);

    fstream->seekp( lastPutPoint );
}

//////////////

// Emplaces world data found from the file stream provided onto the refrence world object.
void REGION::readWorldData(int arrayOffset, std::fstream *fstream, WORLD *world){
    arrayOffset = std::clamp(arrayOffset, 0, 255);
    std::streampos lastGetPoint = fstream->tellg();

    char data[1024] {};

    fstream->seekg( (arrayOffset * 15360) + 2080 );
    fstream->read(data, 1024);

    //We now have the first map, the climate map
    MAP* currentMap = world->getClimatemap();
    int i = 0;
    for (char d : data){
        currentMap->setRaw(i,d);
        i++;
    }

    // Next the heightmap
    fstream->read(data, 1024);
    currentMap = world->getHeightmap();
    i = 0;
    for (char d : data){
        currentMap->setRaw(i,d);
        i++;
    }

    // Finally the saturation map
    fstream->read(data, 1024);
    currentMap = world->getSaturationmap();
    i = 0;
    for (char d : data){
        currentMap->setRaw(i,d);
        i++;
    }

    // Commence reading each cave and emplacing data onto each
    std::array<CAVE, 12>* caves = world->getCaves();

    for (CAVE cave : *caves){
        fstream->read(data, 1024);
        i = 0;
        for (char d : data){
            cave.setRaw(i,d);
            i++;
        }
    }

    fstream->seekg( lastGetPoint );

}

void REGION::writeWorldData(int arrayOffset, std::fstream *fstream, WORLD *world) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    std::streampos lastPutPoint = fstream->tellp();

    char data[1024] {};

    fstream->seekp( (arrayOffset * 15360) + 2080 );

    //We now have the first map, the climate map
    MAP* currentMap = world->getClimatemap();
    for (int i = 0; i < 1024; i++){
        data[i] = currentMap->getRaw(i);
    }
    fstream->write(data, 1024);

    // Next the heightmap
    currentMap = world->getHeightmap();
    for (int i = 0; i < 1024; i++){
        data[i] = currentMap->getRaw(i);
    }
    fstream->write(data, 1024);

    // Finally the saturation map
    for (int i = 0; i < 1024; i++){
        data[i] = currentMap->getRaw(i);
    }
    fstream->write(data, 1024);
    // Commence reading each cave and emplacing data onto each
    std::array<CAVE, 12>* caves = world->getCaves();

    for (CAVE cave : *caves){
        for (int i = 0; i < 1024; i++){
            data[i] = cave.getRaw(i);
        }
        fstream->write(data, 1024);

    }

    fstream->seekp(lastPutPoint );
}