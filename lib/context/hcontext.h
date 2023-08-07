//
// Created by Triage on 5/18/2023.
//

#ifndef HALCYONICUS_HCONTEXT_H
#define HALCYONICUS_HCONTEXT_H
#include "../world/chunk2.h"
#include "../entity/entity.h"
#include <map>
#include <mutex>
#include <set>

//A class defined to assist in holding the gamestate objects required for reading and writing to the current display.


/**
 * When a player loads up my game, they will be introduced to a world beneath their feet able to walk and interact with the world
 *
 * This has to be possible by a few measures, all questions needing answers:
 * - Where is this world?
 * - Where is the player?
 * - What direction is he headed in?
 *
 * Many more questions come of this and we should be able to keep it simple, such as creating the player, world, and entities, then allowing others to read this object where its all stored
 *
 * something important to think about though is threaded saftey. We must ensure all forms of storage are thread safe and fast. Speed is key, and if too much is happening, we could miss-
 * by miss, i mean we could refrence something that isnt there, which would cause many issues. Avoid this.
 */
class HCONTEXT{

    // We want to store a world, but not just one we want a variable amount
    std::map<WORLDCOORD, CHUNK2> chunks{};
    std::mutex worldMutex {};

    //Selective rendering occurs by knowledge of the whereabouts entites are, and when we render these entities, we have to know where they exist in the world before we query them so we dont loop through all of them while working with the data
    std::map< WORLDCOORD, std::map<unsigned long long, ENTITY> > entities{};
    // Get the tuple first then lock the tuple, while unlocking the entity graph
    std::mutex entityMutex{};

    ENTITY player;
    bool isShared = false;
    bool hasEmbeddedPlayer = false;

public:
    //Create a possibly shared instance containing a player
    HCONTEXT(std::string playerName, bool isShared){
        this->hasEmbeddedPlayer = true;
        this->isShared = isShared;
        player = {"player"};
        DYNABLOB name = DYNABLOB(playerName);
        player.setAttribute(name.serialize(), "name");
    }

    //Create a shared instance without a player that the server can use
    HCONTEXT(){
        hasEmbeddedPlayer = false;
        isShared = true;
    }

    bool isSharedInstance(){
        return isShared;
    }

    bool hasPlayerEmbedded(){
        return hasEmbeddedPlayer;
    }

    /*
     * /////////////////
     * World section
     * /////////////////
     */

    /**
     * Whatever is in this area can and will be rendered.
     * @param world
     */
    void insertWorldShard(CHUNK2 world){
        worldMutex.lock();
        chunks.insert({world.getLocation(), world});
        worldMutex.unlock();
    }

    void overrideWorldShard(CHUNK2 world){
        worldMutex.lock();
        auto it = chunks.find(world.getLocation());
        if (it != chunks.end()) {
            // Item found, erase it from the set
            chunks.erase(it);
        }
        chunks.insert({world.getLocation(), world});
        worldMutex.unlock();
    }

    void deleteWorldShardIfExists(WORLDCOORD worldcoord){
        worldMutex.lock();
        auto it = chunks.find(worldcoord);
        if (it != chunks.end()) {
            // Item found, erase it from the set
            chunks.erase(it);
        }
        worldMutex.unlock();
    }

    CHUNK2 * getWorldByLocation(WORLDCOORD worldcoord){
        worldMutex.lock();
        auto it = chunks.find(worldcoord);
        if (it != chunks.end()) {
            // Item found, erase it from the set
            return &it->second;
        }
        worldMutex.unlock();
        return nullptr;
    }

    void deleteAllchunks(){
        worldMutex.lock();
        chunks.clear();
        worldMutex.unlock();
    }

    /*
     * //////////////
     * Entity Section
     * //////////////
     */

    /**
     * Entities should be decoupled from the world, and when we unload a chunk, we may also want to unload entities, however it is not gauranteed to happen and should not happen.
     * We should sort entities by location rather than worldcoord. The reasoning behind this is that when we query to see where it is, we must ensure that the entity
     * can be easily found and then culled if a chunk unloads.
     *
     * If we apply the idea that our entities may move, and therefore update their location here, we can perform 2 operations, one to change their location and scan for their scope periodically,
     * and another to affect their actual location atomically thus allowing us to read from that while having a reliable sorting mechanism too.
     */

    void insertEntity(ENTITY entity) {
        entityMutex.lock();
        WORLDCOORD wc = entity.getLocation().getWorldCoordinates();
        auto it = entities.find(wc);
        if (it == entities.end()) {
            entities.insert({wc, {}});
            it = entities.find(wc);
        }
        it->second.insert({entity.getUUID(), entity});
        entityMutex.unlock();
    }

    //Flat search without hints towards location
    void eraseEntity(unsigned long long uuid) {
        entityMutex.lock();
        for (auto it : entities){
            auto it2 = it.second.find(uuid);
            if (it2 != it.second.end()) it.second.erase(it2);
        }
        entityMutex.unlock();
    }

    void eraseEntity(WORLDCOORD wc, unsigned long long uuid) {
        entityMutex.lock();
        auto it = entities.find(wc);
        if (it != entities.end()){
            it->second.erase(uuid);
        }
        entityMutex.unlock();
    }

    void eraseEntireWorldcoord(WORLDCOORD wc){
        entityMutex.lock();
        entities.erase(wc);
        entityMutex.unlock();
    }

    void moveEntity(unsigned long long uuid){
        //find the entity
        MAPPING m = getEntityIfExists(uuid);
        //If there isn't an entity then just return
        if (m.entity == nullptr) return;

        //Get the current coordinates, where they should be moved
        WORLDCOORD wc = m.entity->getLocation().getWorldCoordinates();
        //Ensure they are not the same as to avoid useless work
        if (wc != m.worldcoord){
            //Get the new area for this to go into
            entityMutex.lock();
            auto it = entities.find(wc);
            //If the new area does not exist then create a new one
            if (it == entities.end()) {
                entities.insert({wc, {}});
                it = entities.find(wc);
            }
            //Move the entity we found from the old worldcoord map to the new worldcoord map
            it->second.insert(std::move(entities.find(m.worldcoord)->second.extract(uuid)));
            entityMutex.unlock();
        }
    }

    struct MAPPING{
        ENTITY* entity;
        WORLDCOORD worldcoord;
    };

    MAPPING getEntityIfExists(unsigned long long uuid){
        entityMutex.lock();
        for (auto it : entities){
            auto it2 = it.second.find(uuid);
            if (it2 != it.second.end()) {
                entityMutex.unlock();
                return {&it2->second, it2->second.getLocation().getWorldCoordinates()};
            }
        }
        entityMutex.unlock();
        return {nullptr, {}};
    }

};
#endif //HALCYONICUS_HCONTEXT_H
