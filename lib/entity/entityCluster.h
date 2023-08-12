//
// Created by Triage on 4/1/2023.
//

#ifndef HALCYONICUS_ENTITYCLUSTER_H
#define HALCYONICUS_ENTITYCLUSTER_H

#include "../world/coordinate.h"
#include "entity.h"


/**
 * Active shards are areas of the world that are already loaded and contain entities. They are entire shards, with all 13 layers
 * and only include entity data. They are workable areas that exist in memory and can be serialized to a blob for saving later
 */
class ENTITYCLUSTER{
    // There should be separate levels for each area, where 0-11 are the caves and then 12 is the overworld

    COORDINATE::WORLDCOORD worldcoord;
public:
    std::map<unsigned long long, ENTITY> entities;
    /**
     * If we want to create an object ready to hold and work with entity data, we should first create a data structure that
     * defines how entities are stored, hence the bare 'areas' structure present, allowing for entities to be added in dynamically
     */
    ENTITYCLUSTER(COORDINATE::WORLDCOORD worldcoord){
        this->worldcoord = worldcoord;
    };


    void addEntity(ENTITY e){
        entities.erase(e.getUUID());
        entities.insert({e.getUUID(), e});
    }

    void removeEntity(unsigned long long uuid){
        entities.erase(uuid);
    }

    ENTITY* getEntity(unsigned long long uuid){
        auto it = entities.find(uuid);
        if (it == entities.end())
            return {};
        else
            return &it->second;
    }

    //FIXME: Needs testing
    std::vector<char> serializeCluster(){
        if (this->entities.empty()){
            std::cout << "Serialization from chunk " << this->worldcoord.x << ", " << this->worldcoord.y << " failed, chunk is empty." << std::endl;
            return {};
        }
        std::cout << "\tSERIAL: " << "Starting up, creating types map.." << std::endl;

        std::map<std::string, std::vector<ENTITY*>> types;
        std::cout << "\tSERIAL: " << "Sorting entities" << std::endl;
        //Sort the types for the entities and bag them together
        for (auto e : entities){
            std::string type = e.second.getType();
            auto it = types.find(type);
            if (it == types.end()){
                types.insert({type, {}});
                it = types.find(type);
            }
            it->second.push_back(&e.second);

        }
        //We should have put together all the types and now we can shuffle them into a sacrificial vector
        std::cout << "\tSERIAL: " << "Creating the vector of chars" << std::endl;
        std::vector<char> out;

        for (auto type : types){
            std::cout << "\tSERIAL: " << "Trying length detection" << std::endl;
            unsigned long realLen = type.first.length();
            auto tLen = (unsigned short) realLen;
            char length[2];
            ::memcpy(length, &tLen, 2);
            std::cout << "\tSERIAL: " << "Implement initial datatype length push" << std::endl;
            //This should in theory copy the value of the short in question to an array of 2 chars instead of taking up
            // a variable amount of space. This can be statically reinterpreted later to give the length for verification
            // purposes

            out.push_back('{');
            out.push_back(length[0]);
            out.push_back(length[1]);
            std::cout << "\tSERIAL: " << "Insert type append" << std::endl;
            out.insert(out.end(), type.first.begin(), type.first.end());
            std::cout << "\tSERIAL: " << "Start serialization of entities" << std::endl;
            for (ENTITY *e : type.second){
                std::cout << "\tSERIAL: " << "Entitity serial of UUID " << e->getUUID() << std::endl;
                std::string entity = e->serializeEntity();
                out.insert(out.end(), entity.begin(), entity.end());
            }
            /*auto it = sev.begin();
            while (it != sev.end()){
                if (it->getType() == type){
                    std::cout << "Preparing to serialize new entity.. "<< std::endl;
                    std::string entity = it->serializeEntity();
                    std::cout << "Found entity as:" << std::endl;
                    outputNeatly(entity);

                    ss << entity;
                    sev.erase(it);
                } else it++;
            }*/
            std::cout << "\tSERIAL: " << "Finalize type \"" << type.first << "\"" << std::endl;
            out.push_back('}');
        }
        //TODO: Try to compress the results with /lib/tools/compressionTools.h
        std::cout << "\tSERIAL: " << "Finished, return the result" << std::endl;
        return out;
    }

    //FIXME: Needs testing
    void deserializeIntoChunk(std::vector<char> &data){

        //TODO: Try to uncompress the results with /lib/tools/compressionTools.h after implementing compression
        if (data.size() <= 2){
            std::cout << "Layer is empty, no changes made." << std::endl;
            return;
        }
        entities.clear();
        //Entities by format are defined by a variable length of characters, however they will always complete at least 2 {} iterations.
        // after the final static }}, we will be able to detect a \n, which if this does not exist, we should seek until we do see one and ignore everything after the }} until we do

        unsigned long position = 0;
        unsigned int bracketCoutner = 0;
        bool typeCounter;
        unsigned long len = data.size();

        while (position < len-1){
            //start by finding the first token using a bracket counter
            while (data[position] != '{') {
                if (position > len-1) {
                    std::cerr << "WARN: Deserialization into chunk completed because we could not find an opener \'{\', last character at position " << position << " was \'" << data[position] << '\'' << std::endl;
                    return;
                }
                position++;
            }
            //We have reached our first bracket and that means we can start recording the first entity type
            std::cout << "Reached first token at position " << position << std::endl;
            typeCounter = true;
            std::stringstream ent;
            std::stringstream type;
            //We are looking for another bracket and recording the characters into the type buffer
            //Raise the position to not count the bracket
            position++;
            std::cout << "Position is now " << position << std::endl;
            unsigned short tLen;
            char tcLen[2];
            tcLen[0] = data[position];
            position++;
            tcLen[1] = data[position];
            position++;
            tLen = *(unsigned short*) tcLen;
            int tracker = 0;
            while (typeCounter){
                if (position > len-1) {
                    std::cout << "INFO: Reached end of file during typecounter step, possibly a malformed string?" << std::endl;
                    return;
                }
                char thisChar = data[position];
                if ( tracker >= tLen) {
                    std::cout << "Entered type " << type.str() << " on char " << data[position] << std::endl;
                    bracketCoutner++;
                    ent << thisChar;
                    position++;
                    while (bracketCoutner != 0){
                        if (position > len-1) {
                            std::cout << "INFO: Reached end of file at position " << position << " with current buffer: " << std::endl << ent.str() << std::endl;
                            return;
                        }
                        thisChar = data[position];
                        ent << thisChar;
                        if (thisChar == '{') bracketCoutner++;
                        if (bracketCoutner > 0){
                            if (thisChar == '}') bracketCoutner--;
                        }
                        position++;
                    }
                    std::cout << "INFO: Found end of entity, emplacing into layer.." << std::endl;
                    ENTITY e = ENTITY::deserializeEntity(ent.str());
                    e.setType(type.str());
                    entities.insert({e.getUUID(), e});
                    std::cout << "INFO: Pushed entity into layer: " << ent.str() << std::endl;
                    ent.str("");
                    ent.clear();
                    //Skip the newline
                    //If we find that this is also a closing bracket we have to assume that this is the end of the stack of entity types
                    if (data[position] == '}') {
                        std::cout << "INFO: reached end of type \"" << type.str() << "\" at position " << position << std::endl;
                        typeCounter = false;
                        type.str("");
                        type.clear();
                        position++;
                    }
                }
                else {
                    type << thisChar;
                    position++;
                    tracker++;
                }

            }
        }
    }

    static void outputNeatly(const std::string& s){
        int bracketcount = 0;
        for (char c : s) {
            if (c == '{'){
                std::cout << std::endl;

                for (int i = 0; i <= bracketcount; i++){
                    std::cout << '\t';
                }
                bracketcount++;
                std::cout << c;
                std::cout << std::endl;
                for (int i = 0; i <= bracketcount; i++){
                    std::cout << '\t';
                }
            } else
            if (c == '}'){
                std::cout << std::endl;
                bracketcount--;
                for (int i = 0; i <= bracketcount; i++){
                    std::cout << '\t';
                }

                std::cout << c;
                std::cout << std::endl;
                for (int i = 0; i <= bracketcount; i++){
                    std::cout << '\t';
                }
            } else std::cout << c;

        }
    }
};
#endif //HALCYONICUS_ENTITYCLUSTER_H
