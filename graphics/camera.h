//
// Created by Triage on 5/22/2023.
//

#ifndef HALCYONICUS_CAMERA_H
#define HALCYONICUS_CAMERA_H

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <SDL_events.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct camera
{
    //Actual data for the camera:
    glm::vec3 position = glm::vec3(0,0,0);
    glm::vec3 direction = glm::vec3(0,0,1);
    float pitch = 0.0;
    float yaw = 0.0;

    float aspectRatio = 1.0;
    float width=1600,height=1200;

    //Quick reference to uniform locations, need to add another set each time you add a different shader program:
    GLint cameraViewMatrix = -1;
    GLint cameraProjectionMatrix = -1;
    GLint cameraPosition = -1;

    void setAspectRatio(float resX,float resY)
    {
        width = resX;
        height = resY;
        aspectRatio = width/height;
    }

    void takeMouseInput(float x,float y)
    {
        pitch -= y * 0.001;
        yaw -= x * 0.001;

        yaw = fmod(yaw,3.1415 * 2.0);

        if(pitch >= 1.57)
            pitch = 1.57;
        if(pitch <= -1.57)
            pitch = -1.57;
    }

    void moveAround(float deltaT = 1.0)
    {
        const Uint8 *states = SDL_GetKeyboardState(NULL);

        float speed = 0.03;

        if(states[SDL_SCANCODE_LCTRL])
            speed *= 5.0;

        if(states[SDL_SCANCODE_LSHIFT])
            position.y -= speed * deltaT;
        if(states[SDL_SCANCODE_SPACE])
            position.y += speed * deltaT;

        if(states[SDL_SCANCODE_W])
            position += direction * speed * deltaT;
        if(states[SDL_SCANCODE_S])
            position -= direction * speed * deltaT;

        float y = atan2(direction.x,direction.z);
        y += 1.57079633;
        y = fmod(y,3.1415 * 2.0);

        if(states[SDL_SCANCODE_D])
            position -= glm::vec3(sin(y),0,cos(y)) * speed * deltaT;
        if(states[SDL_SCANCODE_A])
            position += glm::vec3(sin(y),0,cos(y)) * speed * deltaT;
    }

    void render()
    {
        direction = glm::vec3(cos(pitch)*sin(yaw),sin(pitch),cos(pitch)*cos(yaw));

        //If you want a ragdoll or the ability to lean / tilt, change the upVector from 0,1,0
        glm::mat4 view = glm::lookAt(position,position+direction,glm::vec3(0,1,0));
        //Field of view 90 degrees default, decrease for zoom later
        //Clipping planes are arbitrary, feel free to change
        glm::mat4 projection = glm::perspective(glm::radians(90.0f),aspectRatio,0.1f,1000.0f);

        //Pass matricies to shaders
        glUniformMatrix4fv(cameraViewMatrix,1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(cameraProjectionMatrix,1,GL_FALSE,&projection[0][0]);
        glUniform3f(cameraPosition,position.x,position.y,position.z);
    }

    camera(GLuint program)
    {
        cameraViewMatrix = glGetUniformLocation(program,"cameraViewMatrix");
        cameraProjectionMatrix = glGetUniformLocation(program,"cameraProjectionMatrix");
        cameraPosition = glGetUniformLocation(program,"cameraPosition");
    }
};

#endif //HALCYONICUS_CAMERA_H
