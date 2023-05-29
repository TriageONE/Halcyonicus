//
// Created by Triage on 5/25/2023.
//

#ifndef HALCYONICUS_INITOPENGL_H
#define HALCYONICUS_INITOPENGL_H

#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#define GLEW_STATIC
#define GLEW_NO_GLU
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

SDL_Window *initOpenGL(int resX,int resY);
//Takes two strings, you could load these from files if you want
GLuint createProgram(std::string vertexCode,std::string fragmentCode);
GLuint loadDefaultShaders();

#endif //HALCYONICUS_INITOPENGL_H
