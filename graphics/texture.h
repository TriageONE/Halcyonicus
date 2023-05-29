//
// Created by Triage on 5/25/2023.
//

#ifndef HALCYONICUS_TEXTURE_H
#define HALCYONICUS_TEXTURE_H

#include "initOpengl.h"

enum textureType { none=0, dirt=1, stone=2, wood=3, grass=4 };
std::string textureName(textureType in);

struct texture
{
    bool valid = false;
    GLuint handle;
    int width=0, height=0;

    texture(std::string filePath);
    texture(){};
    ~texture();
};

struct atlas : texture
{
    int atlasRows, atlasColumns;
    int textureWidthAndHeight;
    int numAddedTextures = 0;

    unsigned char *pixelData = 0;

    atlas(int rows,int columns,int textureSize);
    void addFile(std::string filename);
    void compile();
};

#endif //HALCYONICUS_TEXTURE_H
