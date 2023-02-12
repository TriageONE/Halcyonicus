//
// Created by Triage on 2/10/2023.
//
#include <fstream>
#include <iostream>
#include "region.h"

bool REGION::writeChunk(WORLD world) {
    //Should be preceded by check if region exists

    //Must read entire region in order to implement changes to chunks
    std::ifstream regionFile;
    //find the name of the file
    REGIONCOORD regioncoord = findRegioncoordFromWorldShard(&world);
    //Parse it to a filename
    std::stringstream filename;
    filename << "./world/" << parseRegioncoordToFname(regioncoord);

    regionFile.open(filename.str());

    if(regionFile.is_open()){

    }
    return true;
}

WORLD REGION::readChunk(WORLD::WORLDCOORD worldcoord) {
    return WORLD("", LOCATION());
}

bool REGION::worldExists(WORLD::WORLDCOORD worldcoord) {
    return false;
}

void REGION::writeChunks(std::vector<WORLD> worlds) {

}

std::vector<WORLD> REGION::readChunks(std::vector<WORLD::WORLDCOORD> worldcoords) {
    return std::vector<WORLD>();
}

bool REGION::regionExists(REGION::REGIONCOORD regioncoord) {
    std::ifstream regionFile;
    std::stringstream name;
    name << "rg_" << std::to_string(regioncoord.x) << "_" << std::to_string(regioncoord.y) << ".hcr";
    regionFile.open(name.str());
    return regionFile.is_open();
}

std::string REGION::parseRegioncoordToFname(REGION::REGIONCOORD regioncoord) {
    std::stringstream name;
    name << "rg_" << std::to_string(regioncoord.x) << "_" << std::to_string(regioncoord.y) << ".hcr";
    return name.str();
}

REGION::REGIONCOORD REGION::parseFnameToRegioncoord(const std::string& fname) {
    int x=0, y=0;
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
    }
    return {x, y};
}

REGION::REGIONCOORD REGION::findRegioncoordFromWorldShard(WORLD *world) {
    LOCATION l = world->getLocation();
    return {l.getX() >> 4, l.getZ() >> 4};
}

int REGION::findChunkArrayOffset(LOCATION chunkLocation) {
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


void REGION::setChunkExists(int arrayOffset, std::fstream *fstream) {

    //Save the last points, since there may have been an interaction that might've taken place before this requiring a placement
    std::streampos lastGetPoint = fstream->tellg();
    std::streampos lastPutPoint = fstream->tellp();

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data;

    unsigned int writePoint = 32 + (arrayOffset / 8);
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

//Set the existence table with a bit that lets us know that the chunk at this place exists






