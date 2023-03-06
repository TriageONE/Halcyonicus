//
// Created by Aron Mantyla on 2/18/23.
//

#ifndef HALCYONICUS_REGIONCOORD_H
#define HALCYONICUS_REGIONCOORD_H

#include "coordinate.h"

class REGIONCOORD : public COORDINATE{
public:
    REGIONCOORD(int xInt, int yInt){
        this->x = xInt;
        this->y = yInt;
    }

};
#endif //HALCYONICUS_REGIONCOORD_H
