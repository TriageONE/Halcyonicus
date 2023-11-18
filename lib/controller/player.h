//
// Created by Aron Mantyla on 10/18/23.
//

#ifndef HALCYONICUS_PLAYER_H
#define HALCYONICUS_PLAYER_H

#include <glm/vec2.hpp>
#include "../entity/entity.h"
#include "../logging/hlogger.h"
using namespace hlogger;
class PLAYER{

public:
    ENTITY* pe;

    float moveSpeedLimit = 4;

    PLAYER(double x, double y, float z, std::string name){
        pe = new ENTITY({x, y, z}, "player");
        pe->setAttribute({name}, "name");
    };


};
#endif //HALCYONICUS_PLAYER_H
