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

    PLAYER() = default;

    /**
     * Takes in a number between -1 and 1 for either mX or mY to affect the position of the entity
     * @param mX
     * @param mY
     * @param deltaTime
     */
    void move(float mX, float mY, float deltaTime){
        long long   x = mX * 1000 * moveSpeedLimit * (deltaTime /1000),
                    y = mY * 1000 * moveSpeedLimit * (deltaTime /1000);
        pe->getLocation().manipulate(x, y, 0);
        info << "New location is " << pe->getLocation().x << ", " << pe->getLocation().y << nl;
    }

};
#endif //HALCYONICUS_PLAYER_H
