//
// Created by Triage on 2/10/2023.
//
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include "region.h"
#include "regioncoord.h"
#include <filesystem>
#ifdef WIN32
    #include <fileapi.h>
    #include <windows.h>
    #include <direct.h>
#endif



bool REGION::writeChunk(WORLD* world) {
    //Should be preceded by check if region exists

    //Must read entire region in order to implement changes to chunks
    std::fstream regionFile;
    //find the name of the file
    REGIONCOORD regioncoord = findRegioncoordFromWorldShard(world);
    //Parse it to a filename

    std::filesystem::path path = prependWorldDir(parseRegioncoordToFname(regioncoord));

    int arrayOffset = findChunkArrayOffset(world->getLocation());

    regionFile.open(path);
    if (!(regionFile)) return false;
    using namespace std;
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

        setHash(arrayOffset, &regionFile, hash);
    } else {
        regionFile.close();
        return false;
    }
    regionFile.close();
    return true;
}

bool REGION::readChunk(WORLD* world) {

    //Affect the linked world to be overwritten with new data from the file
    //Must read entire region in order to implement changes to chunks
    std::fstream regionFile;
    //find the name of the file
    REGIONCOORD regioncoord = findRegioncoordFromWorldShard(world);
    //Parse it to a filename
    std::filesystem::path path = prependWorldDir(parseRegioncoordToFname(regioncoord));


    int arrayOffset = findChunkArrayOffset(world->getLocation());

    regionFile.open(path);
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
        std::cout << "Reading " << world->getLocation().getX() << ", " << world->getLocation().getY() << std::endl;
        readWorldData(arrayOffset, &regionFile, world);
    } else return false;
    regionFile.close();
    return true;

}

/////////////////////
/// File operations

/**
 * Creates the directories that are used to hold worlds, entities and other things. Will not change current contents but may be appended to in the future. comes with checks
 */

#if defined(WIN32)
using namespace std;
string ExePath() {
    char buffer[MAX_PATH] = { 0 };
    GetModuleFileName( nullptr, buffer, MAX_PATH );
    string s = string{buffer};
    unsigned long long pos = s.find_last_of("\\/");
    return s.substr(0, pos);
}

bool DirectoryExists(LPCTSTR lpszDirectoryPath)
{
    struct _stat buffer{};
    int iRetTemp = 0;

    memset ((void*)&buffer, 0, sizeof(buffer));

    iRetTemp = _stat(lpszDirectoryPath, &buffer);

    if (iRetTemp == 0){
        if (buffer.st_mode & _S_IFDIR) return true;
        else return false;
    }
    else return false;
}
#endif

bool REGION::createDirectories(){
    bool completed = true;
    #if defined(WIN32)
    //WIN32
    const array<LPCSTR, 4> dirs {"\\world", "\\world\\entities", "\\world\\players", "\\world\\data"};
    string currentPath = ExePath();
    for (const LPCSTR path : dirs) {
        string absPath = currentPath;
        absPath.append(path);
        if (CreateDirectoryA(absPath.c_str(), nullptr)){
            std::cout << "Created " << absPath << std::endl;
        }
    }
    #else
    //POSIX

        const std::array<std::filesystem::path, 4> dirs {"./world", "./world/entities", "./world/players", "./world/data"};

        struct stat s{};

        for (const std::filesystem::path& path : dirs){
            std::cout << "Checking for " << path << std::endl;
            stat(path.c_str(), &s);

            if (!S_ISDIR(s.st_mode)){
                //Create a directory for the world that is readable and writable for us but not others
                int dErr = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

                std::cout << "Attempted create on " << path << " with code " << dErr << std::endl;
                if (dErr == 0) {
                    std::cout << "Created " << path << std::endl;
                    continue;
                }
                std::cerr << "DISK_IO ERROR, CLASS WORLD_INIT; " << path << " Directory creation failed, mkdir error returned \'" << dErr << "\' and ERRNO " << errno << ", currently in " << std::filesystem::current_path() << std::endl;
                completed = false;
            }
        }
    #endif
    return completed;
}

/**
 * Will return the state of the directory structure
 * @return True if the directories exist
 */
bool REGION::checkForDirectoryStructure(){
    using namespace std;
    #if defined(WIN32)
    const array<LPCSTR, 4> dirs {"\\world", "\\world\\entities", "\\world\\players", "\\world\\data"};
    string currentPath = ExePath();

    for (const LPCSTR path : dirs) {
        string absPath = currentPath;
        absPath.append(path);
        cout << "Checking " << absPath << endl;
        if (DirectoryExists(absPath.c_str())) {
            cout << "Path was a valid directory: " << absPath << endl;
            continue;
        }
        cout << "Directory " << absPath << " does not exist!" << endl;
        return false;
    }
    #else
    std::array<std::filesystem::path, 4> dirs {"./world", "./world/entities", "./world/players", "./world/data"};
    struct stat s{};
    for (const std::filesystem::path& path : dirs){
        cout << "Checking for " << path << endl;
        stat(path.c_str(), &s);
        if (!S_ISDIR(s.st_mode)){
            cout << "Path of " << path << " does not exist!" << endl;
            return false;
        }
    }
    #endif

    return true;
}

void REGION::createEmptyWorld(const std::filesystem::path& path){
    //In order to prime the file for storage, we should initially create a new file with the name and nothing in it
    std::fstream file;
    file.open(path);

    using namespace std;

    //Create new file if not exists
    if (!file.is_open())
    {
        cout << "Creating new file \'" << path << endl;
        file.clear();
        file.open(path, std::ios::out); // create file

        cout << "good:" << file.good() << "; open:" << file.is_open() << endl;

        //After creating the empty file, we should write a 32 bit section of nothing to indicate that the maps are not generated
        std::filesystem::resize_file(path,3934241);
        //Now the file's existence table should be zeroed successfully

        // we must prime the complete size by moving the write-head to the last byte of the file and writing an examble, or an ending bit of 0x55
        // To find the final bit, we take 256 maps times 15360 per map plus an offset of 2080 additive equalling 3934240, and so 3934241 would be our final byte to examble.


        // The letter U is represented as hex char 0x55, a perfect 0b 0101 0101 for testing. when reading, we can test for legitimacy by assuming if there is a U at the eof then its likely a HCX file.
        // We have time to change this, but we should also include maybe a version and a type header.. that may come in handy. A forward hash may also be helpful, possibly in dealing with corruption
        file.seekp(3934241);
        file.write("U", 1);
        file.close();
        cout << "Finalized new file \'" << path << endl;

    } else {
        std::cerr << "DISK_IO WARNING, ATTEMPT TO CREATE EMPTY WORLD OVER ALREADY EXISTING WORLD; ATTEMPTED ON: " << path << "; ATTEMPT WAS REJECTED, NO CHANGES MADE";
    }

}

bool REGION::worldExists(WORLDCOORD worldcoord) {
    //A region coordinate indicates an entire region, however a worldcoord indicates a region within that, and that was represented by an existence table
    //Find the region file first
    std::fstream regionFile;
    REGIONCOORD regioncoord = worldcoord.getRegionCoordinates();
    //Parse it to a filename
    std::filesystem::path path = prependWorldDir(parseRegioncoordToFname(regioncoord));

    int arrayOffset = findChunkArrayOffset(worldcoord);

    regionFile.open(path);
    if (!(regionFile)) return false;

    return chunkExists(arrayOffset, &regionFile);
}

bool REGION::regionExists(REGIONCOORD regioncoord) {
    std::ifstream regionFile;
    std::stringstream name;
    name << "rg_" << std::to_string(regioncoord.getX()) << "_" << std::to_string(regioncoord.getY()) << ".hcr";
    string absPath = prependWorldDir(name.str());

    #if defined(WIN32)
        DWORD stat = GetFileAttributesA(absPath.c_str());
        return (stat != INVALID_FILE_ATTRIBUTES && !(stat & FILE_ATTRIBUTE_DIRECTORY));
    #else
        bool isOpen;
        regionFile.open();
        bool isOpen = regionFile.is_open();
        if (isOpen) regionFile.close();
        return isOpen;
    #endif


}

////////////////////////////////////
/// File interpretation and parsing

std::string REGION::prependWorldDir(const std::string& in) {
    std::stringstream ss;
    #if defined(WIN32)
        string currentPath = ExePath();
        ss << currentPath << "\\world\\" << in;
    #else
        ss << "./world/" << in;
    #endif
    return ss.str();
}

std::string REGION::parseRegioncoordToFname(REGIONCOORD regioncoord) {
    std::stringstream name;
    name << "rg_" << std::to_string(regioncoord.getX()) << "_" << std::to_string(regioncoord.getY()) << ".hcr";
    return name.str();
}

std::filesystem::path REGION::parseFullPathFromRegionCoord(REGIONCOORD regioncoord){
    std::filesystem::path p;
    p = REGION::prependWorldDir(REGION::parseRegioncoordToFname(regioncoord));
    return p;
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
    return {l.getX() >> 4, l.getY() >> 4};

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
    int y = chunkLocation.getY();

    int xoff = x & 15;
    int yoff = y & 15;

    return (yoff * 16) + xoff;
}

//////////////////////////////////
/// Database tools

//FIXME: This does not work, the bit never gets set as this gets hung on write
void REGION::setChunkExists(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    //Save the last points, since there may have been an interaction that might've taken place before this requiring a placement

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data;

    unsigned int writePoint = (arrayOffset / 8);

    fstream->seekg( writePoint ); // If we just divide the array position by the length of the data, we can get exactly the offset needed to read, plus 32 for the start of the data
    fstream->read(&data, 1);

    //we have the byte, now we mask it and OR it with a special value
    const char comp = 1;
    data = data | (comp << (arrayOffset % 8));

    //Write the data to the file
    fstream->seekp( writePoint ); // Same for put position, we will be writing to the same spot

    fstream->write(&data, 1);
    fstream->flush();

}

char REGION::chunkExists(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);


    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data;

    unsigned int readPoint = (arrayOffset / 8);

    fstream->seekg( readPoint ); // If we just divide the array position by the length of the data, we can get exactly the offset needed to read, plus 32 for the start of the data
    fstream->seekp( readPoint );

    fstream->read(&data, 1);

    // Should read as:
    //    If the bit at the position is a 1, then return true. Evaluation to anything other than 0 is true.
    const char comp = 1;
    char offset = (arrayOffset % 8);
    return (data & (comp << offset));
}

//////////////////

int REGION::getHash(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    char data[4];

    fstream->seekg( (arrayOffset * 4) + 32 );
    fstream->seekp( (arrayOffset * 4) + 32 );

    fstream->read(data, 4);

    // Should read as:
    //    If the bit at the position is a 1, then return true. Evaluation to anything other than 0 is true.
    return *(int*) data;
}

void REGION::setHash(int arrayOffset, std::fstream *fstream, int data) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    char d2[4]{0};
    ::memcpy(d2, &data, 4);

    //define a char to be refrenced later and written to by the reader, then read from by the writer
    fstream->seekp( (arrayOffset * 4) + 32 );
    fstream->seekg( (arrayOffset * 4) + 32 );

    fstream->write(d2, 4);
}

//////////

int REGION::getTimestamp(int arrayOffset, std::fstream *fstream) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    char data[4] {};


    fstream->seekg( (arrayOffset * 4) + 1056 );
    fstream->seekp( (arrayOffset * 4) + 1056 );

    fstream->read(data, 4);

    return *(int*) data; //Dangerous typecast to integer. Maybe this wont work? i guess if we do this the exact same way on write then it doesnt matter right
    // I tested in a scratch file and it seems that this wants to work. Only thing we need to be certain of is the data length.

}

void REGION::setTimestamp(int arrayOffset, std::fstream *fstream, int timestamp) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);
    char data[4] {};
    ::memcpy(data,&timestamp,4);

    fstream->seekp( (arrayOffset * 4) + 1056 );
    fstream->seekg( (arrayOffset * 4) + 1056 );
    fstream->write(data, 4);
}

//////////////

// Emplaces world data found from the file stream provided onto the refrence world object.
void REGION::readWorldData(int arrayOffset, std::fstream *fstream, WORLD *world){
    arrayOffset = std::clamp(arrayOffset, 0, 255);

    char data[1024] {};

    fstream->seekg( (arrayOffset * 15360) + 2080 );
    fstream->seekg( (arrayOffset * 15360) + 2080 );

    using namespace std;

    fstream->read(data, 1024);
    MAP *currentMap = &world->climatemap;
    int i = 0;
    for (char d : data){
        currentMap->setRaw(i,d);
        i++;
    }

    // Next the heightmap
    fstream->read(data, 1024);
    currentMap = &world->heightmap;
    i = 0;
    for (char d : data){
        currentMap->setRaw(i,d);
        i++;
    }

    // Finally the saturation map
    fstream->read(data, 1024);
    currentMap = &world->saturationmap;
    i = 0;
    for (char d : data){
        currentMap->setRaw(i,d);
        i++;
    }

    // Commence reading each cave and emplacing data onto each
    int cn = 0;
    for (CAVE &cave : world->caves){
        fstream->read(data, 1024);

        i = 0;
        for (char d : data){
            cave.setRaw(i,d);
            i++;
        }
        cn++;
    }
}

void REGION::writeWorldData(int arrayOffset, std::fstream *fstream, WORLD *world) {
    arrayOffset = std::clamp(arrayOffset, 0, 255);


    char data[1024] {};

    fstream->seekp( (arrayOffset * 15360) + 2080 );
    fstream->seekg( (arrayOffset * 15360) + 2080 );

    using namespace std;

    //We now have the first map, the climate map
    MAP *currentMap = &world->climatemap;
    for (int i = 0; i < 1024; i++){
        data[i] = currentMap->getRaw(i);
    }
    fstream->write(data, 1024);

    // Next the heightmap
    currentMap = &world->heightmap;
    for (int i = 0; i < 1024; i++){
        data[i] = currentMap->getRaw(i);
    }
    fstream->write(data, 1024);

    // Finally the saturation map
    currentMap = &world->saturationmap;

    for (int i = 0; i < 1024; i++){
        data[i] = currentMap->getRaw(i);
    }
    fstream->write(data, 1024);

    for (CAVE cave : world->caves){
        for (int i = 0; i < 1024; i++){
            data[i] = cave.getRaw(i);
        }
        fstream->write(data, 1024);
    }
}