//
// Created by Aron Mantyla on 10/27/23.
//

#ifndef HALCYONICUS_ENTITY3D_H
#define HALCYONICUS_ENTITY3D_H
#include "../../lib/entity/entity.h"
#include "../Model.h"

class ENTITY3D{
public:
    static void draw(ENTITY* entity, Shader* shader, Model* model, glm::mat4* view, glm::mat4* projection){
        if (!model->loaded) model->Load();
        glm::mat4 m = glm::scale(glm::vec3(1, 1, 1));	// scale it
        m = glm::toMat4(glm::quat(1.0,0.0,entity->getFacing(),0.0)) * m; // Rotate it
        auto el = entity->getLocation();
        m = glm::translate(glm::vec3(el.x / 1000.0,el.z, el.y / 1000.0)) * m; // Move it
        //Enable the shader
        shader->use();

        //Assign the values to the shader uniforms
        shader->setMat4("projection", *projection);
        shader->setMat4("view", *view);
        shader->setMat4("model", m);
        model->Draw(*shader);
    }
};
#endif //HALCYONICUS_ENTITY3D_H
