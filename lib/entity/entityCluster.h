//
// Created by Triage on 4/1/2023.
//

#ifndef HALCYONICUS_ENTITYCLUSTER_H
#define HALCYONICUS_ENTITYCLUSTER_H

#include <utility>
#include <vector>
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
    
};
#endif //HALCYONICUS_ENTITYCLUSTER_H
