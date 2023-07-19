//
// Created by Triage on 5/25/2023.
//

#ifndef HALCYONICUS_INITOPENGL_H
#define HALCYONICUS_INITOPENGL_H

#include "halcyonicus.h"

SDL_Window *initOpenGL(int resX,int resY);
//Takes two strings, you could load these from files if you want
GLuint createProgram(std::string vertexCode,std::string fragmentCode);
GLuint loadDefaultShaders(std::string vertexShaderPath,std::string fragmentShaderPath);

#endif //HALCYONICUS_INITOPENGL_H
