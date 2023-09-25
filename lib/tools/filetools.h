//
// Created by Triage on 9/1/2023.
//

#ifndef HALCYONICUS_FILETOOLS_H
#define HALCYONICUS_FILETOOLS_H
#ifdef WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif
#include "../logging/hlogger.h"
#include "../world/coordinate.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace hlogger;
class FTOOLS{

#if(WIN32)
    static constexpr auto dirs = {"\\world", "\\world\\entities", "\\world\\players", "\\world\\data", "\\world\\blocks", "\\world\\data"};
#else
    static constexpr auto dirs = {"./world", "./world/entities", "./world/players", "./world/data", "./world/blocks", "./world/data"};
#endif

public:
    enum TYPE {
        TERRAIN,
        ENTITY,
        BLOCK,
        DATA
    };
    // Create filesystem
    static bool createDirectories(){
        bool completed = true;
        #if defined(WIN32)
        //WIN32
        std::string currentPath = exePath();
        for (const LPCSTR path : dirs) {
            std::string absPath = currentPath;
            absPath.append(path);
            if (CreateDirectoryA(absPath.c_str(), nullptr)){
                info << "Created " << absPath << std::endl;
            }
        }
        #else
        //POSIX
        struct stat s{};

        for (const std::filesystem::path& path : dirs){
            info << "Checking for " << path << std::endl;
            stat(path.c_str(), &s);

            if (!S_ISDIR(s.st_mode)){
                //Create a directory for the world that is readable and writable for us but not others
                int dErr = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

                info << "Attempted create on " << path << " with code " << dErr << std::endl;
                if (dErr == 0) {
                    info << "Created " << path << std::endl;
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
     * Will return the state of the directory structure. Handles all directories within the world, including the data for the entities, entities themselves and the players
     * @return True if the directories exist
     */
    static bool checkForDirectoryStructure(){
        si;
        #if defined(WIN32)
        std::string currentPath = exePath();

        for (const LPCSTR path : dirs) {
            std::string absPath = currentPath;
            absPath.append(path);
            info << "Checking " << path << std::endl;
            if (directoryExists(absPath.c_str())) {
                info << "Path valid: " << path << std::endl;
                continue;
            }
            warn << "Directory " << path << " does not exist!" << std::endl;
            so;
            return false;
        }
        #else
        struct stat s{};
        for (const std::filesystem::path& path : dirs){
            info << "Checking for " << path.c_str()  << std::endl;
            stat(path.c_str(), &s);
            if (!S_ISDIR(s.st_mode)){
                warn << "Path of " << path  << " does not exist!" << std::endl;
                so;
                return false;
            }
        }
        #endif
        so;
        return true;
    }

    #if defined(WIN32)
    static std::string exePath() {
        char buffer[MAX_PATH] = { 0 };
        GetModuleFileName( nullptr, buffer, MAX_PATH );
        std::string s = std::string{buffer};
        unsigned long long pos = s.find_last_of("\\/");
        return s.substr(0, pos);
    }

    static bool directoryExists(LPCTSTR lpszDirectoryPath)
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

    // Prepend entity directory
    static std::string prependDirectory(const std::string& in, TYPE type) {
        std::stringstream ss;
        std::string t;
        switch (type) {
            case TERRAIN:
                t = "world";
                break;
            case ENTITY:
                t = "entities";
                break;
            case BLOCK:
                t = "blocks";
                break;
            case DATA:
                t = "data";
                break;
        }
        #if defined(WIN32)
        ss << ".\\world\\" << t << "\\" << in;
        #else
        ss << "./world/" << t << "/" << in;
        #endif
        return ss.str();
    }

    //Parsers for naming
    static std::string parseRegioncoordToFilename(COORDINATE::REGIONCOORD regioncoord, TYPE type) {
        std::stringstream name;
        char modifier;
        switch (type) {
            case TERRAIN:
                modifier = 't';
                break;
            case ENTITY:
                modifier = 'e';
                break;
            case BLOCK:
                modifier = 'b';
                break;
            case DATA:
                modifier = 'd';
                break;
        }
        name << "rg" << modifier << "_" << std::to_string(regioncoord.x) << "_" << std::to_string(regioncoord.y) << ".hdb";
        return name.str();
    }

    static std::string parseFullPathFromRegionCoord(COORDINATE::REGIONCOORD regioncoord, TYPE type){
        return prependDirectory(parseRegioncoordToFilename(regioncoord, type),type);
    }

    static COORDINATE::REGIONCOORD parseDBFileToRegioncoord(const std::string& fname) {
        int x, y;
        try{
            if (!(fname[0] == 'r' && fname[1] == 'g' && (fname[2] == 'e' || fname[2] == 't' || fname[2] == 'd' || fname[2] == 'b') && fname[3] == '_')){
                throw std::domain_error(R"(Does not match the required named convention, must start with "rg_"!)");
            }
            std::stringstream ss;
            const std::string comp = "0123456789-";

            short tr = 4;
            for (int i = 3; ;i++){
                if (comp.find(fname[i]) != std::string::npos) ss << fname[i];
                if (fname[i] == '_') break;
                if (i > 14) throw std::out_of_range(R"(First coordinate X does not match the required named convention, must not exceed millions place!)");
                tr++;
            }
            x = std::stoi(ss.str());
            ss.clear();
            for (;;tr++){
                if (comp.find(fname[tr]) != std::string::npos) ss << fname[tr];
                if (fname[tr] == '.') break;
                if (tr > 26) throw std::out_of_range(R"(Second coordinate Y does not match the required named convention, must not exceed millions place!)");
            }
            y = std::stoi(ss.str());
            ss.clear();
        } catch (std::exception &e) {
            err << "File parsing failed, standard convention error: " << e.what() << std::endl;
            return {0,0};
        }
        return {x, y};
    }


};
#endif //HALCYONICUS_FILETOOLS_H
