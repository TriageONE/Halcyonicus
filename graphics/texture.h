//
// Created by Triage on 5/25/2023.
//

#ifndef HALCYONICUS_TEXTURE_H
#define HALCYONICUS_TEXTURE_H

#include <vector>
#include "initOpengl.h"

enum textureType { none=0, dirt=1, stone=2, wood=3, grass=4 };
std::string textureName(textureType in);

struct texture
{
    GLenum textureType = GL_TEXTURE_2D;
    bool valid = false;
    GLuint handle;
    bool created = false;
    bool mipMaps = true;
    bool isHdr = false;
    int resX = 0,resY = 0,layers=0;

    GLenum magFilter =   GL_LINEAR;
    GLenum minFilter =   GL_LINEAR_MIPMAP_LINEAR;
    GLenum texWrapS =    GL_CLAMP_TO_EDGE;
    GLenum texWrapT =    GL_CLAMP_TO_EDGE;
    GLenum texWrapR =    GL_CLAMP_TO_EDGE;

    void allocateDepth(unsigned int width,unsigned int height,unsigned int _layers,GLenum internalFormat,GLenum format,GLenum type);
    void allocateDepth(unsigned int width,unsigned int height,GLenum internalFormat = GL_DEPTH24_STENCIL8,GLenum format = GL_DEPTH_STENCIL,GLenum type = GL_UNSIGNED_INT_24_8);
    void useForFramebuffer(GLenum attachmentNumber);
    void setFilter(GLenum _magFilter,GLenum _minFilter);
    void setWrapping(GLenum texWrap);
    void setWrapping(GLenum _texWrapS,GLenum _texWrapT,GLenum _texWrapR);
    void setParams(GLenum _textureType,int w,int h);
    void useMipMaps(bool value);
    glm::vec2 getResolution();
    void allocate(bool hdr,unsigned int width,unsigned int height,unsigned int channels);

    texture(std::string filePath);
    texture(){};
    ~texture();
};

struct renderTarget
{
    struct renderTargetSettings
    {
        int resX = 800;
        int resY = 800;
        unsigned int layers = 0; //set to 0 for GL_TEXTURE_2D
        bool HDR = false;
        int numColorChannels = 3;
        bool useStencil = false;
        bool useColor = true;
        bool useDepth = true;
        GLenum depthBufferType = GL_DEPTH_COMPONENT;
        glm::vec4 clearColor = glm::vec4(0,0,0,1);
        GLenum texWrapping = GL_CLAMP_TO_EDGE;
        GLenum minFilter = GL_LINEAR;
        GLenum magFilter = GL_LINEAR;
    } settings;

    texture *colorResult = 0;
    texture *depthResult = 0;
    GLuint frameBuffer = 0;
    GLuint depthBuffer = 0;
    GLenum *drawBuffers = 0;

    renderTarget(renderTargetSettings &frameBufferPrefs);
    ~renderTarget();

    void bind();
    void unbind();
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

struct textureArray : texture
{
    std::vector<unsigned char> pixelData;

    void addFile(std::string filename);
    void compile();
};

#endif //HALCYONICUS_TEXTURE_H
