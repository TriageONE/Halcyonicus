//
// Created by Triage on 10/13/2023.
//

#ifndef HALCYONICUS_ENTITY3D_H
#define HALCYONICUS_ENTITY3D_H

#include "../Model.h"
#include "../../lib/entity/entity.h"
#include "glm/gtx/quaternion.hpp"

class ENTITY3D{

    float facing = 0.0, scale = 1.0;

    ENTITY* entity;
    Model* model;

    explicit ENTITY3D(Model* model) : model{model} {
        if (!model->loaded) model->Load();
    };

    void draw(Shader* shader){
        glm::mat4 model = glm::scale(glm::vec3(scale, scale, scale));	// scale it
        model = glm::toMat4(glm::quat(1.0,0.0,facing,0.0)) * model; // Rotate it
        auto el = entity->getLocation();
        model = glm::translate(glm::vec3(el.x / 1000, el.y / 1000, el.z / 100)) * model; // Move it
        this->model->Draw(*shader, model);
    }

};


#endif //HALCYONICUS_ENTITY3D_H
