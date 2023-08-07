//
// Created by Aron Mantyla on 8/2/23.
//

#ifndef HALCYONICUS_IOBJECT3D_H
#define HALCYONICUS_IOBJECT3D_H

#include "../../graphics/Model.h"

/**
 * <h3>
 * Class IObject3D (Interactive object 3D)
 * </h3>
 * The primary purpose of this class is to create a 3d object to be represented in the real world, with clearly defined and transparent qualities
 * such qualities include:
 * <li>
 *  <ul>A clearly defined location, rotation and scale
 *  <ul>A hit/physics-detection bounding box, strictly defined from a width and height and offset given via entity descriptor
 *  <ul>An optionally tracked animation progress scale
 * </li>
 */

class IOBJECT3D{

    //Contains the center of the model calculated by deriving the middle of the extremes
    float xCenter, zCenter;

    //The model the object should use
    Model model;

public:
    float x, y, z;
    float yaw, pitch, roll;

    IOBJECT3D(Model *model){
        this->model = model;
    }


};


#endif //HALCYONICUS_IOBJECT3D_H
