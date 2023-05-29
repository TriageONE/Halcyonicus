//
// Created by Triage on 5/25/2023.
//

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string textureName(textureType in)
{
    switch(in)
    {
        case none: return "air";
        case dirt: return "dirt";
        case stone: return "stone";
        case wood: return "wood";
        case grass: return "grass";
        default: return "air";
    }
}

atlas::atlas(int rows,int columns,int textureSize) : texture()
{
    atlasRows = rows;
    atlasColumns = columns;
    textureWidthAndHeight = textureSize;
    numAddedTextures = 0;
    pixelData = new unsigned char[atlasRows * atlasColumns * textureWidthAndHeight * textureWidthAndHeight * 4];

    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
        std::cout<<"Error in atlas::atlas(rows,columns,textureSize) "<<err<<"\n";
}

void atlas::addFile(std::string fileName)
{
    std::cout<<"Trying to load "<<fileName<<"\n";

    int channels;
    unsigned char *data = stbi_load(fileName.c_str(),&width,&height,&channels,0);

    if(height == 0 || width == 0 || !data)
    {
        std::cout<<"Tried to load "<<fileName<<" height, width, or pointer was zero\n";
        return;
    }

    if(channels < 3 || channels > 4)
    {
        std::cout<<"Texture "<<fileName<<" needs to have 3 or 4 channels, it had "<<channels<<"\n";
        return;
    }

    if(height != textureWidthAndHeight)
    {
        std::cout<<"Texture atlas wants source textures to have height and width of "<<textureWidthAndHeight<<" but added file had height "<<height<<"\n";
        return;
    }

    if(width != textureWidthAndHeight)
    {
        std::cout<<"Texture atlas wants source textures to have height and width of "<<textureWidthAndHeight<<" but added file had width "<<width<<"\n";
        return;
    }

    //Atlas expects 4 channel RGBA textures, if we lack an alpha channel, we will create one with a default opacity of 100%
    if(channels == 3)
    {
        unsigned char *newData = new unsigned char[width*height*4];
        for(int a = 0; a<width*height; a++)
        {
            newData[a*4] = data[a*3];
            newData[a*4+1] = data[a*3+1];
            newData[a*4+2] = data[a*3+2];
            newData[a*4+3] = 255;
        }

        delete data;
        data = newData;
        channels = 4;
    }

    int pixelDataStartX = (numAddedTextures % atlasColumns) * textureWidthAndHeight;
    int pixelDataStartY = (numAddedTextures / atlasColumns) * textureWidthAndHeight;

    for(int x = 0; x<width; x++)
    {
        for(int y = 0; y<height; y++)
        {
            int offset = pixelDataStartX + x;
            offset += (pixelDataStartY + y) * (atlasColumns * textureWidthAndHeight);

            pixelData[offset*4]   = data[(x + y * width) * 4];
            pixelData[offset*4+1] = data[(x + y * width) * 4+1];
            pixelData[offset*4+2] = data[(x + y * width) * 4+2];
            pixelData[offset*4+3] = data[(x + y * width) * 4+3];
        }
    }

    numAddedTextures++;
    delete data;

    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
        std::cout<<"Error in atlas::addFile(file) "<<err<<"\n";
}

void atlas::compile()
{
    if(numAddedTextures < 1 || !pixelData)
    {
        std::cout<<"atlas::compile called before any textures added!\n";
        return;
    }

    valid = true;
    glGenTextures(1,&handle);

    glBindTexture(GL_TEXTURE_2D,handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    width = textureWidthAndHeight * atlasColumns;
    height = textureWidthAndHeight * atlasRows;
    std::cout<<"Creating texture with width: "<<width<<" and height: "<<height<<"\n";
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixelData);
    glBindTexture(GL_TEXTURE_2D,0);

    delete pixelData;
    pixelData = 0;

    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
        std::cout<<"Error in atlas::compile() "<<err<<"\n";
}

texture::texture(std::string filePath)
{
    int channels;
    unsigned char *data = stbi_load(filePath.c_str(),&width,&height,&channels,0);

    if(height == 0 || width == 0 || !data)
    {
        std::cout<<"Tried to load "<<filePath<<" height, width, or pointer was zero\n";
        return;
    }

    if(channels < 3 || channels > 4)
    {
        std::cout<<"Texture "<<filePath<<" needs to have 3 or 4 channels, it had "<<channels<<"\n";
        return;
    }

    valid = true;
    glGenTextures(1,&handle);

    glBindTexture(GL_TEXTURE_2D,handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,0,channels == 3 ? GL_RGB : GL_RGBA,width,height,0,channels == 3 ? GL_RGB : GL_RGBA,GL_UNSIGNED_BYTE,data);
    glBindTexture(GL_TEXTURE_2D,0);

    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
        std::cout<<"Error in texture::texture(file) "<<err<<"\n";
}

texture::~texture()
{
    if(valid)
        glDeleteTextures(1,&handle);
}
