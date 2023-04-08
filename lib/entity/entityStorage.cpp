//
// Created by Triage on 4/3/2023.
//

#include "entityStorage.h"
#include "set"
using namespace std;
/*string ENTITYSTORAGE::serializeEntity(std::vector<ENTITY> entities) {
    string output;
    //We should find all types of one entity and pack them into a group of entities of the same class, then give them their unique attrs
    map<string, std::vector<ENTITY>> organizer;

    // Organize all the entities by type
    for (ENTITY en : entities){
        if (!organizer.contains(en.getType())) {
            // mymap.insert ( std::pair<char,int>('a',100) );
            organizer.insert(pair<string, std::vector<ENTITY>>(en.getType(), vector<ENTITY>{en}));
        } else {
            // std::map<char,int>::iterator it;
            auto place = organizer.find(en.getType());
            place->second.push_back(en);
        }
    }

    // Serialize the entities in chunks so that we can pack data together easily based on a template


}*/