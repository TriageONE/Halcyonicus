//
// Created by Aron Mantyla on 2/18/23.
//

#ifndef HALCYONICUS_REGIONCOORD_H
#define HALCYONICUS_REGIONCOORD_H

#include "../../deprecated/coordinate.h"

class REGIONCOORD : public COORDINATE{
public:
    REGIONCOORD(int xInt, int zInt){
        this->x = xInt;
        this->z = zInt;
    }

};
#endif //HALCYONICUS_REGIONCOORD_H
