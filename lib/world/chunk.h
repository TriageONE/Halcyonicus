//
// Created by Triage on 5/24/2023.
//

#ifndef HALCYONICUS_CHUNK_H
#define HALCYONICUS_CHUNK_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "worldcoord.h"

#define layout_chunk_vertexPositions 0
#define layout_chunk_vertexNormals 1
#define layout_chunk_vertexUVs 2
#define layout_chunk_cubeData 3
#define layout_chunk_typeData 4

#define chunk_allocation_size 50

struct chunk;

struct cube_location{unsigned char a,b,c;};

//TODO: IM GONNA https://www.youtube.com/watch?v=GAfAer_sjaE
struct  cube
{
    //These four values are what data our cube actually consists of:
    unsigned char offsetX = 0;
    unsigned char offsetY = 0;
    unsigned char offsetZ = 0;
    unsigned short type = 0;

    //How we know where on the graphics card our cube is:
    chunk *parent = 0;
    unsigned short bufferOffset = 0;

    cube_location packed(){
        return cube_location(offsetX, offsetY, offsetZ);
    }
};


/**
 * Blocks need a type, a location xy and z,
 * type is a short, 2 bytes,
 * location x and z vary from 0-15 therefore .4 byte each so now 3 bytes total
 * a 4th byte for the Y value
 * Is this all we need?
 */
struct chunk
{
    //This is allocated once at startup and shared among all cubes:
    static GLuint vertexPositions;
    static GLuint vertexNormals;
    static GLuint vertexUVs;
    static bool vertexPositionsCreated;  //I hope stack overflow was right when it said this defaulted to false lol
    static GLint uniformChunkX,uniformChunkZ;
    static void createStaticBuffers(GLuint program);
    GLuint vao;
    GLuint cubeData;
    GLuint typeData;
    unsigned int allocatedCubeSpaces = 0;
    unsigned int allocatedCubeTypes = 0;

    unsigned char *serialize(int &size);
    void load(unsigned char *serial,int size);

    //DONE: Adapt rest of class to use standard worldcoord
    WORLDCOORD location= {0,0};

    //Why do we need 2 types of cube maps? wouldnt it be easier to have a map of cubes if we dont really need a vector?
    std::map<unsigned short, cube> cubeMap;
    std::vector<cube*> cubes;
    chunk(GLuint program, int x, int z);
    ~chunk();

    //If changing a lot, call changeCube with skipCompile = true, then call recompileAll after
    //If changing like one cube, just call changeCube normally and don't call recompileAll after
    void recompileAll();
    const cube getCube(unsigned char x,unsigned char y,unsigned char z);
    void changeCube(cube *tmp,unsigned char type,bool skipCompile = false);
    void changeCube(unsigned char x, unsigned char y, unsigned char z, unsigned short type, bool skipCompile = false);
    void render();

    //Triage section
    std::vector<int> serialize2();

    void deserialize2(std::vector<int> data);
};

#endif //HALCYONICUS_CHUNK_H
