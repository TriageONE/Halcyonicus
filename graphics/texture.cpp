//
// Created by Triage on 5/25/2023.
//

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLenum resolveChannels(int channels,bool hdr)
{
    switch(channels)
    {
        case 1: return hdr ? GL_R16F    : GL_RED;
        case 2: return hdr ? GL_RG16F   : GL_RG;
        case 3: return hdr ? GL_RGB16F  : GL_RGB;
        case 4: return hdr ? GL_RGBA16F : GL_RGBA;
    }

    return 0;
}

void texture::useForFramebuffer(GLenum attachmentNumber)
{
    if(textureType == GL_TEXTURE_2D)
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentNumber, textureType, handle,0);
    else
        glFramebufferTexture(GL_FRAMEBUFFER,attachmentNumber,handle,0);
}

void texture::allocateDepth(unsigned int width,unsigned int height,GLenum internalFormat,GLenum format,GLenum type)
{
    resX = width;
    resY = height;
    glBindTexture(textureType,handle);
    glTexImage2D(textureType, 0, internalFormat, width, height, 0, format, type, NULL);
    setParams(textureType,resX,resY);
    glBindTexture(textureType,0);
}

void texture::allocateDepth(unsigned int width,unsigned int height,unsigned int _layers,GLenum internalFormat,GLenum format,GLenum type)
{
    layers = _layers;
    resX = width;
    resY = height;

    glBindTexture(textureType,handle);
    glTexImage3D(textureType, 0, internalFormat, width, height,layers, 0, format, type, NULL);
    setParams(textureType,resX,resY);
    glBindTexture(textureType,0);
}

glm::vec2 texture::getResolution()
{
    return glm::vec2(resX,resY);
}

void texture::useMipMaps(bool value)
{
    /*scope("texture::useMipMaps");
    if(created)
        error("Call to texture::useMipMaps after creation!");*/
    mipMaps = value;
}

void texture::setFilter(GLenum _magFilter,GLenum _minFilter)
{
    /*scope("texture::setFilter");
    if(created)
        error("Call to texture::setFilter after creation!");*/
    magFilter = _magFilter;
    minFilter = _minFilter;
}

void texture::setWrapping(GLenum _texWrapS,GLenum _texWrapT,GLenum _texWrapR)
{
    //scope("texture::setWrapping");
    //if(created)
      //  error("Call to texture::setWrapping after creation!");
    texWrapS = _texWrapS;
    texWrapT = _texWrapT;
    texWrapR = _texWrapR;
}

void texture::setWrapping(GLenum texWrap)
{
    setWrapping(texWrap,texWrap,texWrap);
}

void texture::setParams(GLenum _textureType,int w,int h)
{
    //scope("texture::setParams");

    textureType = _textureType;
    resX = w;
    resY = h;

    if(minFilter != GL_NEAREST && minFilter != GL_LINEAR && !mipMaps)
    {
        //error("No mipmaps for texture, disabling.");
        minFilter = GL_LINEAR;
    }

    glBindTexture(textureType,handle);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_S, texWrapS);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_T, texWrapT);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_R, texWrapR);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, minFilter);
    glBindTexture(textureType,0);
}

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

void textureArray::addFile(std::string fileName)
{
    if(valid)
    {
        std::cout<<"Tried to add file to compiled texture array!\n";
        return;
    }

    int channels,fileWidth,fileHeight;
    unsigned char *data = stbi_load(fileName.c_str(),&fileWidth,&fileHeight,&channels,0);

    if(fileWidth == 0 || fileHeight == 0 || channels == 0)
    {
        std::cout<<"Error loading "<<fileName<<"\n";
        return;
    }

    if(layers == 0)
    {
        resX = fileWidth;
        resY = fileHeight;
    }
    else
    {
        if(resX != fileWidth || resY != fileHeight)
        {
            std::cout << "Dimensions of file " << fileName << " need to be " << resX << "," << resY << "\n";
            return;
        }
    }

    if(channels != 3 && channels != 4)
    {
        std::cout<<"Texture "<<fileName<<" needs to have 3 or 4 channels!\n";
        return;
    }

    //Texture array expects 4 channel RGBA textures, if we lack an alpha channel, we will create one with a default opacity of 100%
    if(channels == 3)
    {
        unsigned char *newData = new unsigned char[resX*resY*4];
        for(int a = 0; a<resX*resY; a++)
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

    //copy(&data[0],&data[width*height*4],back_inserter(pixelData));
    pixelData.insert(pixelData.end(),&data[0],&data[resX*resY*4]);
    layers++;
}

void textureArray::compile()
{
    if(valid)
    {
        std::cout<<"Cannot compile texture array twice!\n";
        return;
    }

    if(layers == 0)
    {
        std::cout<<"Tried to compile empty texture array!\n";
        return;
    }

    if(pixelData.size() != 4*resX*resY*layers)
    {
        std::cout<<"Pixel data contained "<<pixelData.size()<<" bytes when it should have "<<4*resX*resY*layers<<" byte!\n";
        return;
    }

    valid = true;
    glGenTextures(1,&handle);

    glBindTexture(GL_TEXTURE_2D_ARRAY,handle);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY,1,GL_RGBA8,resX,resY,layers);

    int tmpWidth = resX,tmpHeight = resY,mip=0;
    while(tmpWidth >= 1 && tmpHeight >= 1)
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,mip,0,0,0,tmpWidth,tmpHeight,layers,GL_RGBA,GL_UNSIGNED_BYTE,&pixelData[0]);
        tmpWidth >>= 1;
        tmpHeight >>= 1;
        mip++;
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    pixelData.clear();

    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
        std::cout<<"Error in textureArray::compile() "<<err<<"\n";
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
    unsigned char *data = stbi_load(fileName.c_str(),&resX,&resY,&channels,0);

    if(resX == 0 || resY == 0 || !data)
    {
        std::cout<<"Tried to load "<<fileName<<" height, width, or pointer was zero\n";
        return;
    }

    if(channels < 3 || channels > 4)
    {
        std::cout<<"Texture "<<fileName<<" needs to have 3 or 4 channels, it had "<<channels<<"\n";
        return;
    }

    if(resY != textureWidthAndHeight)
    {
        std::cout<<"Texture atlas wants source textures to have height and width of "<<textureWidthAndHeight<<" but added file had height "<<resY<<"\n";
        return;
    }

    if(resX != textureWidthAndHeight)
    {
        std::cout<<"Texture atlas wants source textures to have height and width of "<<textureWidthAndHeight<<" but added file had width "<<resX<<"\n";
        return;
    }

    //Atlas expects 4 channel RGBA textures, if we lack an alpha channel, we will create one with a default opacity of 100%
    if(channels == 3)
    {
        unsigned char *newData = new unsigned char[resX*resY*4];
        for(int a = 0; a<resX*resY; a++)
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

    for(int x = 0; x<resX; x++)
    {
        for(int y = 0; y<resY; y++)
        {
            int offset = pixelDataStartX + x;
            offset += (pixelDataStartY + y) * (atlasColumns * textureWidthAndHeight);

            pixelData[offset*4]   = data[(x + y * resX) * 4];
            pixelData[offset*4+1] = data[(x + y * resX) * 4+1];
            pixelData[offset*4+2] = data[(x + y * resX) * 4+2];
            pixelData[offset*4+3] = data[(x + y * resX) * 4+3];
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    resX = textureWidthAndHeight * atlasColumns;
    resY = textureWidthAndHeight * atlasRows;
    std::cout<<"Creating texture with width: "<<resX<<" and height: "<<resY<<"\n";
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,resX,resY,0,GL_RGBA,GL_UNSIGNED_BYTE,pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);
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
    unsigned char *data = stbi_load(filePath.c_str(),&resX,&resY,&channels,0);

    if(resY == 0 || resX == 0 || !data)
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

    glTexImage2D(GL_TEXTURE_2D,0,channels == 3 ? GL_RGB : GL_RGBA,resX,resY,0,channels == 3 ? GL_RGB : GL_RGBA,GL_UNSIGNED_BYTE,data);
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

void texture::allocate(bool hdr,unsigned int width,unsigned int height,unsigned int channels)
{
    //scope("texture::allocate");
    if(created)
    {
        //error("Texture already created.");
        return;
    }

    isHdr = hdr;
    resX = width;
    resY = height;

    /*if(isHdr)
        hdrData = new float[resX*resY*channels];
    else
        ldrData = new unsigned char[resX*resY*channels];*/

    glBindTexture(textureType,handle);
    glTexImage2D(
            textureType,
            0,
            resolveChannels(channels,isHdr),
            resX,
            resY,
            0,
            resolveChannels(channels,false),
            isHdr ? GL_FLOAT : GL_UNSIGNED_BYTE,
            isHdr ? (void*)0 : (void*)0);
    setParams(textureType,resX,resY);
    glBindTexture(textureType,0);
}

renderTarget::renderTarget(renderTargetSettings &frameBufferPrefs)
{
    //scope("renderTarget::renderTarget");
    settings = frameBufferPrefs;

    glGenFramebuffers(1,&frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    std::cout<<"Layers: "<<frameBufferPrefs.layers<<"\n";

    if(settings.useColor)
    {
        /*if(frameBufferPrefs.layers != 0)
            colorResult = new texture(GL_TEXTURE_2D_ARRAY);
        else
            colorResult = new texture(GL_TEXTURE_2D);*/
        colorResult = new texture();

        colorResult->setWrapping(settings.texWrapping);
        colorResult->setFilter(settings.magFilter,settings.minFilter);
        /*if(frameBufferPrefs.layers != 0)
            colorResult->allocate(settings.HDR,settings.resX,settings.resY,frameBufferPrefs.layers,settings.numColorChannels);
        else*/
            colorResult->allocate(settings.HDR,settings.resX,settings.resY,settings.numColorChannels);
        colorResult->useForFramebuffer(GL_COLOR_ATTACHMENT0);    //glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentNumber, GL_TEXTURE_2D, handle,0);
        drawBuffers =  new GLenum[1];
        drawBuffers[0] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);
    }
    else
        glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(settings.depthBufferType != GL_FALSE)
    {
        if(settings.useDepth)
        {

            std::cout<<"Using depth, layers: "<<frameBufferPrefs.layers<<"\n";
            /*if(frameBufferPrefs.layers!=0)
            {
                depthResult = new texture(GL_TEXTURE_2D_ARRAY);
                glBindTexture(GL_TEXTURE_2D_ARRAY,depthResult->handle);
                glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_DEPTH_COMPONENT32F,settings.resX,settings.resY,frameBufferPrefs.layers,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthResult->handle, 0);

            }
            else
            {*/
                /*if(frameBufferPrefs.layers != 0)
                    depthResult = new texture(GL_TEXTURE_2D_ARRAY);
                else
                    depthResult = new texture(GL_TEXTURE_2D);*/
                depthResult = new texture();

                depthResult->setWrapping(settings.texWrapping);
                depthResult->setFilter(settings.magFilter,settings.minFilter);
                /*if(frameBufferPrefs.layers != 0)
                    depthResult->allocateDepth(settings.resX,settings.resY,frameBufferPrefs.layers,settings.depthBufferType,settings.depthBufferType,GL_FLOAT);        //glTexImage2D
                else*/
                    depthResult->allocateDepth(settings.resX,settings.resY,settings.depthBufferType,settings.depthBufferType,GL_FLOAT);        //glTexImage2D
                depthResult->useForFramebuffer(settings.useStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT);
            //}

        }
        else
        {
            glGenRenderbuffers(1, &depthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, settings.depthBufferType, settings.resX,settings.resY);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, settings.useStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
        }
    }
    /*else if(settings.useDepth)
        error("useDepth needs to be false if settings.depthBufferType is GL_FALSE");*/


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout<<"Framebuffer creation error: " << std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)) << " : " << std::to_string(glGetError())<<"\n";
        //error("Framebuffer creation error: " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)) + " : " + std::to_string(glGetError()));
    /*else
        debug("Framebuffer created!");*/


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

renderTarget::~renderTarget()
{
    glDeleteFramebuffers(1,&frameBuffer);
    glDeleteRenderbuffers(1,&depthBuffer);
    delete colorResult;
    delete depthResult;
    delete drawBuffers;
}

void renderTarget::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
    glViewport(0,0,settings.resX,settings.resY);
    glClearColor( settings.clearColor.r,settings.clearColor.g,settings.clearColor.b,settings.clearColor.a );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderTarget::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}