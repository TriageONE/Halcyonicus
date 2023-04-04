//
// Created by Triage on 4/1/2023.
//

#include "entityCluster.h"

ENTITYCLUSTER::ENTITYCLUSTER(WORLDCOORD w) {
    this->worldcoord = w;
}

WORLDCOORD ENTITYCLUSTER::getWorldcoord() {
    return this->worldcoord;
}



