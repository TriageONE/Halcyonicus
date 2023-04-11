//
// Created by Triage on 4/1/2023.
//

#ifndef HALCYONICUS_ENTITYCLUSTER_H
#define HALCYONICUS_ENTITYCLUSTER_H

#include <utility>
#include <vector>
#include <algorithm>
#include <fstream>
#include "entity.h"
#include "../world/world.h"
#include "../world/region.h"

/**
 * Active shards are areas of the world that are already loaded and contain entities. They are entire shards, with all 13 layers
 * and only include entity data. They are workable areas that exist in memory and can be serialized to a blob for saving later
 */
class ENTITYCLUSTER{
    // There should be separate levels for each area, where 0-11 are the caves and then 12 is the overworld

    WORLDCOORD worldcoord;
public:
    std::array<std::vector<ENTITY>, 13> areas {std::vector<ENTITY>()};
    /**
     * If we want to create an object ready to hold and work with entity data, we should first create a data structure that
     * defines how entities are stored, hence the bare 'areas' structure present, allowing for entities to be added in dynamically
     */
    explicit ENTITYCLUSTER(WORLDCOORD w);

    WORLDCOORD getWorldcoord();

    void addEntityToLayer(const ENTITY& e, int layer){
        if (layer > 12 || layer < 0) return;
        areas[layer].push_back(e);
    }

    string serializeLayer(int layer){
        if (this->areas[layer].empty()){
            cout << "Serialization from layer " << layer << " failed, layer is empty." << endl;
            return {""};
        }
        std::clamp(layer, 0, 12);

        std::vector<string> types;

        //Sort the types for the entities and bag them together eventually
        for (ENTITY e : this->areas[layer]){
            string type = e.getType();
            if (std::find(types.begin(), types.end(), type) == types.end())
                types.push_back(type);
        }
        //We should have put together all the types and now we can shuffle them into a sacrificial vector

        vector<ENTITY> sev = vector<ENTITY>(this->areas[layer]);

        stringstream ss;

        for (const string& type : types){
            unsigned long realLen = type.length();
            auto tLen = (unsigned short) realLen;
            char length[2];
            ::memcpy(length, &tLen, 2);

            //This should in theory copy the value of the short in question to an array of 2 chars instead of taking up
            // a variable amount of space. This can be statically reinterpreted later to give the length for verification
            // purposes

            ss << "{" << length[0] << length[1] << type.substr(0, tLen );
            cout << "Current SS: " << ss.str() << endl;
            auto it = sev.begin();
            while (it != sev.end()){
                if (it->getType() == type){
                    cout << "Preparing to serialize new entity.. "<< endl;
                    string entity = it->serializeEntity();
                    cout << "Found entity as:" << endl;
                    outputNeatly(entity);

                    ss << entity;
                    sev.erase(it);
                } else it++;
            }
            ss << '}';
        }

        return ss.str();
    }

    void deserializeIntolayer(const string& data, int layer){
        if (data.length() <= 2){
            cout << "Layer is empty, no changes made." << endl;
            return;
        }
        std::clamp(layer, 0, 12);
        areas[layer].clear();
        //Entities by format are defined by a variable length of characters, however they will always complete at least 2 {} iterations.
        // after the final static }}, we will be able to detect a \n, which if this does not exist, we should seek until we do see one and ignore everything after the }} until we do

        unsigned long position = 0;
        unsigned int bracketCoutner = 0;
        bool typeCounter;
        unsigned long len = data.length();

        while (position < len-1){
            //start by finding the first token using a bracket counter
            while (data[position] != '{') {
                if (position > len-1) {
                    cerr << "WARN: Deserialization into layer " << layer << " completed because we could not find an opener \'{\', last character at position " << position << " was \'" << data[position] << '\'' << endl;
                    return;
                }
                position++;
            }
            //We have reached our first bracket and that means we can start recording the first entity type
            cout << "Reached first token at position " << position << endl;
            typeCounter = true;
            stringstream ent;
            stringstream type;
            //We are looking for another bracket and recording the characters into the type buffer
            //Raise the position to not count the bracket
            position++;
            cout << "Position is now " << position << endl;
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
                    cout << "INFO: Reached end of file during typecounter step, possibly a malformed string?" << endl;
                    return;
                }
                char thisChar = data[position];
                if ( tracker >= tLen) {
                    cout << "Entered type " << type.str() << " on char " << data[position] << endl;
                    bracketCoutner++;
                    ent << thisChar;
                    position++;
                    while (bracketCoutner != 0){
                        if (position > len-1) {
                            cout << "INFO: Reached end of file at position " << position << " with current buffer: " << endl << ent.str() << endl;
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
                    cout << "INFO: Found end of entity, emplacing into layer.." << endl;
                    ENTITY e = ENTITY::deserializeEntity(ent.str());
                    e.setType(type.str());
                    areas[layer].push_back(e);
                    cout << "INFO: Pushed entity into layer: " << ent.str() << endl;
                    ent.str("");
                    ent.clear();
                    //Skip the newline
                    //If we find that this is also a closing bracket we have to assume that this is the end of the stack of entity types
                    if (data[position] == '}') {
                        cout << "INFO: reached end of type \"" << type.str() << "\" at position " << position << endl;
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

    static void outputNeatly(const string& s){
        int bracketcount = 0;
        for (char c : s) {
            if (c == '{'){
                cout << endl;

                for (int i = 0; i <= bracketcount; i++){
                    cout << '\t';
                }
                bracketcount++;
                cout << c;
                cout << endl;
                for (int i = 0; i <= bracketcount; i++){
                    cout << '\t';
                }
            } else
            if (c == '}'){
                cout << endl;bracketcount--;
                for (int i = 0; i <= bracketcount; i++){
                    cout << '\t';
                }

                cout << c;
                cout << endl;
                for (int i = 0; i <= bracketcount; i++){
                    cout << '\t';
                }
            } else cout << c;

        }
    }
};
#endif //HALCYONICUS_ENTITYCLUSTER_H
