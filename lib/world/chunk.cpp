//
// Created by Triage on 5/24/2023.
//
#include <iostream>
#include <bit>
#include <bitset>
#include "chunk.h"

GLuint chunk::vertexPositions = 0;
GLuint chunk::vertexNormals = 0;
GLuint chunk::vertexUVs = 0;
bool chunk::vertexPositionsCreated = false;  //I hope stack overflow was right when it said this defaulted to false lol
GLint chunk::uniformChunkX = 0,chunk::uniformChunkZ = 0;


//Triage section

/*
 * You need the following for a block:
 *  4 bits for x
 *  4 bits for z
 *  8 bits for y
 *  16 bits for the type
 *  xz      y-      type----
 *  XX      XX      XX    XX
 *  Each X is 4 bits
 *  A line means it occupies that space from where it started
 */

struct cube_traits{
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned short type;
};

int serializeBlock(unsigned char x, unsigned char y, unsigned char z, short type){
    unsigned char xz = (x << 4) + z;
    return type + (y << 16) + (xz << 24);
}

int serializeBlock(cube_traits traits){
    unsigned char xz = (traits.x << 4) + traits.z;
    return traits.type + (traits.y << 16) + (xz << 24);
}

unsigned short keyBlock(unsigned char x, unsigned char y, unsigned char z){
    unsigned char xz = (x << 4) + z;
    return y + (((int)(xz)) << 8);
}

unsigned short keyBlock(cube_traits traits){
    unsigned char xz = (traits.x << 4) + traits.z;
    return traits.y + (((int)(xz)) << 8);
}

cube_traits unkeyBlock(int block){
    unsigned char x, y, z;

    y = (block) & 0x00FF;
    x = (block >> 8) & 0x000F;
    z = (block >> 12) & 0x000F;

    return {x, y, z, 0};
}

cube_traits deserializeBlock(int block){
    unsigned char x, y, z;
    unsigned short type = block & 65535;

    y = (block >> 16) & 0x00FF;
    x = (block >> 24) & 0x000F;
    z = (block >> 28) & 0x000F;

    return {x, y, z, type};
}

std::vector<int> chunk::serialize2(){
    std::vector<int> buffer;

    for (auto it : cubeMap){
        auto c = it.second;
        cube_traits traits(c.offsetX, c.offsetY, c.offsetZ, c.type);
        int sResult = serializeBlock(traits);
        buffer.push_back(sResult);
    }
    return buffer;
}

void chunk::deserialize2(std::vector<int> data){
    //TODO: clear the entire chunk since deserializing into an object should override the entire object
    for (int i : data) {
        cube_traits traits = deserializeBlock(i);
        changeCube(traits.x, traits.y, traits.z, traits.type, true);
    }
    recompileAll();
}

//Drendran section
[[deprecated]]unsigned char *chunk::serialize(int &size)
{
    std::vector<unsigned char> buffer;

    int zerosInARow = 0;
    for(unsigned int x = 0; x<16; x++)
    {
        for(unsigned int y = 0; y<256; y++)
        {
            for(unsigned char z = 0; z<16; z++)
            {
                unsigned short key = (((unsigned short)x)<<4) | (((unsigned short)y)<<8) | z;
                /*cube *tmp = cubeMap.count(key) ? cubeMap.at(key) : 0;
                if(!tmp || tmp->type == 0)
                {
                    if(zerosInARow == 255)
                    {
                        buffer.push_back(0);
                        buffer.push_back(zerosInARow);
                        zerosInARow = 0;
                    }
                    else
                        zerosInARow++;
                }
                else
                {
                    if(zerosInARow > 0)
                    {
                        buffer.push_back(0);
                        buffer.push_back(zerosInARow);
                        zerosInARow = 0;
                    }
                if(tmp)
                {
                    std::cout << "Cube type " << (int)tmp->type << " at " << x << "," << y << "," << z << "\n";
                    buffer.push_back(tmp->type);
                }
                else
                    buffer.push_back(0);
                //}*/
            }
        }
    }

    unsigned char *ret = new unsigned char [buffer.size()];
    //std::copy(buffer.begin(),buffer.end(),ret);
    for(int a = 0; a<buffer.size(); a++)
        ret[a] = buffer[a];
    size = buffer.size();
    return ret;
}

[[deprecated]]void chunk::load(unsigned char *serial, int size)
{
    unsigned int x=0,y=0,z=0;
    unsigned int iter = 0;
    while(iter < size)
    {
        /*if(serial[iter] == 0)
        {
            iter++;
            for(int i = 0; i<serial[iter]; i++)
            {
                changeCube(x,y,z,0,true);
                z++;
                if(z >= 16)
                {
                    z = 0;
                    y++;
                    if(y >= 256)
                    {
                        y = 0;
                        x++;
                        if(x >= 16)
                        {
                            std::cout<<"Error, coords were "<<x<<","<<y<<","<<z<<" Iter: "<<iter<<"\n";
                            return;
                        }
                    }
                }
            }
        }
        else
        {*/
            if(serial[iter] != 0)
                std::cout<<"Loaded: "<<(int)serial[iter]<<" - "<<x<<","<<y<<","<<z<<"\n";

            changeCube(x,y,z,serial[iter],true);
            z++;
            if(z >= 16)
            {
                z = 0;
                y++;
                if(y >= 256)
                {
                    y = 0;
                    x++;
                    if(x >= 16)
                    {
                        std::cout<<"Error, coords were "<<x<<","<<y<<","<<z<<" Iter: "<<iter<<"\n";
                        return;
                    }
                }
            }
       // }

        iter++;
    }

    recompileAll();
}

const cube chunk::getCube(unsigned char x,unsigned char y,unsigned char z)
{
    unsigned short key = keyBlock(x,y,z);//(x<<4) | (y<<8) | z;
    auto it = cubeMap.find(key);
    if (it == cubeMap.end()){
        //We diddnt find one, so its air
        return {x, y, z, 0};
    }
    return it->second;
}

void chunk::changeCube(cube *tmp,unsigned char type,bool skipCompile)
{
    tmp->type = type;
    glBindBuffer(GL_ARRAY_BUFFER,cubeData);
    cube_location update = tmp->packed();
    glBufferSubData(GL_ARRAY_BUFFER,tmp->bufferOffset * 3,3,&update);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindBuffer(GL_ARRAY_BUFFER,typeData);
    glBufferSubData(GL_ARRAY_BUFFER,tmp->bufferOffset,4,&type);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void chunk::changeCube(unsigned char x,unsigned char y,unsigned char z,unsigned short type,bool skipCompile)
{
    unsigned short key = keyBlock(x,y,z);//(x<<4) | (y<<8) | z;

    auto it = cubeMap.find(key);

    if (it == cubeMap.end()){
        //We didnt find a cube..
        if (type != 0)
        {
            cubeMap.insert({key, cube{x,y,z,type}});
            it = cubeMap.find(key);

            if(skipCompile)
                return;
        }
        else
        {
            //TODO: ensure that changeCube() removes entries upon seeing a zero in type

        }
    }
    else
    {
        it->second.type = type;
        if(skipCompile)
            return;

        if(cubeMap.size() + 1 >= allocatedCubeSpaces)
        {
            recompileAll();
            return;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER,typeData);
    glBufferSubData(GL_ARRAY_BUFFER,it->second.bufferOffset,2,&type);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindBuffer(GL_ARRAY_BUFFER,cubeData);
    cube_location update = it->second.packed();
    glBufferSubData(GL_ARRAY_BUFFER,it->second.bufferOffset * 3,3,&update);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    /*
    cube *tmp = cubeMap.count(key) ? cubeMap.at(key) : 0;
    //If there's no cube and we didn't want one there anyway...
    if(!tmp && type == 0)
        return;
    else if(tmp)
    {
        //If there is a cube, just change its type:
        tmp->type = type; //Reference by pointer does not work since you arent actually referencing the object within the map, you need an iterator for that
        glBindBuffer(GL_ARRAY_BUFFER,cubeData);
        cube_location update = tmp->packed();
        glBufferSubData(GL_ARRAY_BUFFER,tmp->bufferOffset * 3,3,&update);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        //It doesn't matter even if we change the type to 0
        //The shader discards cubes with a type of 0, even if they still exist
        //The memory for said cubes will be cleared with the next recompileAll call
        //TODO: cubeMap never recieves an update if this branch executes
        return;
    }
    //There is no cube, but we'd like one...

    tmp = new cube;
    tmp->offsetX = x;
    tmp->offsetY = y;
    tmp->offsetZ = z;
    tmp->type = type;

    std::cout<<"Setting: "<<(int)tmp->offsetX<<","<<(int)tmp->offsetY<<","<<(int)tmp->offsetZ<<" - "<<(int)tmp->type<<"\n";

    tmp->bufferOffset = cubes.size();
    cubes.push_back(tmp);
    cubeMap[key] = tmp;

    if(skipCompile)
        return;

    if(cubes.size() + 1 >= allocatedCubeSpaces)
    {
        recompileAll();
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER,cubeData);
    cube_location update = tmp->packed();
    glBufferSubData(GL_ARRAY_BUFFER,tmp->bufferOffset * 4,4,&update);
    glBindBuffer(GL_ARRAY_BUFFER,0);*/
}

//Should we really be using the cube vector here? we have a cube map already that gives locations based off keys to our cubes

void chunk::recompileAll()
{

    //Erase all air blocks from memory
    for(auto it = cubeMap.begin(); it != cubeMap.end();)
    {
        if(it->second.type == 0)
            it = cubeMap.erase(it);
        else ++it;
    }

    std::vector<cube_location> locations;
    std::vector<short> types;
    int tracker = 0;
    for(auto it : cubeMap)
    {
        it.second.bufferOffset = tracker;
        locations.push_back(it.second.packed());
        types.push_back(it.second.type);
        tracker++;
    }

    //What is this doing here? do we need it? it seems to be allocating useless space to our chunk..
    //for(int a = 0; a<chunk_allocation_size; a++)
        //locations.push_back({0,0,0});

    //Bind array buffer for the cube location data

    allocatedCubeSpaces = locations.size();
    glBindBuffer(GL_ARRAY_BUFFER,cubeData); //Following line can also maybe be changed to dynamic draw:
    glBufferData(GL_ARRAY_BUFFER, 3 * locations.size(), locations.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //Bind array buffer for the cube location data
    glBindBuffer(GL_ARRAY_BUFFER,typeData); //Following line can also maybe be changed to dynamic draw:
    glBufferData(GL_ARRAY_BUFFER, 2 * types.size(), types.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

}

void chunk::render()
{
    if(allocatedCubeSpaces < 1)
        return;

    glUniform1f(uniformChunkX,location.getX());
    glUniform1f(uniformChunkZ,location.getZ());

    glBindVertexArray(vao);
    //glDrawArraysInstanced(GL_TRIANGLE_STRIP,0,14,allocatedCubeSpaces);
    glDrawArraysInstanced(GL_TRIANGLES,0,36,allocatedCubeSpaces);
    glBindVertexArray(0);
}

chunk::chunk(GLuint program,int x,int z)
{
    createStaticBuffers(program);
    location.setX(x);
    location.setZ(z);

    //Allocate handle for openGL
    glGenVertexArrays(1,&vao);

    //Most of the constructor is just setting up the VAO
    glBindVertexArray(vao);

    //First we need to re-use the list of verticies for our cube for each chunk:
    glBindBuffer(GL_ARRAY_BUFFER,vertexPositions);
    glEnableVertexAttribArray(layout_chunk_vertexPositions);
    glVertexAttribDivisor(layout_chunk_vertexPositions,0);
    glVertexAttribPointer(layout_chunk_vertexPositions,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    //Re-use normals too:
    glBindBuffer(GL_ARRAY_BUFFER,vertexNormals);
    glEnableVertexAttribArray(layout_chunk_vertexNormals);
    glVertexAttribDivisor(layout_chunk_vertexNormals,0);
    glVertexAttribPointer(layout_chunk_vertexNormals,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    //And UVs
    glBindBuffer(GL_ARRAY_BUFFER,vertexUVs);
    glEnableVertexAttribArray(layout_chunk_vertexUVs);
    glVertexAttribDivisor(layout_chunk_vertexUVs,0);
    glVertexAttribPointer(layout_chunk_vertexUVs,2,GL_FLOAT,GL_FALSE,0,(void*)0);

    //Now we also create the chunk-specific list of data for all of the chunk's cubes:
    glGenBuffers(1,&cubeData);
    glBindBuffer(GL_ARRAY_BUFFER,cubeData);
    allocatedCubeSpaces = chunk_allocation_size;
    glBufferData(GL_ARRAY_BUFFER,3 * allocatedCubeSpaces,NULL,GL_STREAM_DRAW); //should probably be dynamic, be sure to test both ways

    //Configure the cubeData buffer's use further:
    glEnableVertexAttribArray(layout_chunk_cubeData);
    glVertexAttribDivisor(layout_chunk_cubeData,1);
    glVertexAttribIPointer(layout_chunk_cubeData,3,GL_UNSIGNED_BYTE,0,0);

    //What about types now? i made another layout for the type, not a uniform
    //Now we also create the chunk-specific list of data for all of the chunk's cubes:
    glGenBuffers(1,&typeData);
    glBindBuffer(GL_ARRAY_BUFFER,typeData);
    allocatedCubeTypes = chunk_allocation_size;
    glBufferData(GL_ARRAY_BUFFER, allocatedCubeTypes,NULL,GL_STREAM_DRAW); //should probably be dynamic, be sure to test both ways

    //Configure the cubeData buffer's use further:
    glEnableVertexAttribArray(layout_chunk_typeData);
    glVertexAttribDivisor(layout_chunk_typeData,1);
    glVertexAttribIPointer(layout_chunk_typeData,1,GL_UNSIGNED_SHORT,0,0);
}

chunk::~chunk()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(1,&cubeData);
}

void chunk::createStaticBuffers(GLuint program)
{
    if(vertexPositionsCreated)
        return;

    uniformChunkX = glGetUniformLocation(program,"chunkX");
    uniformChunkZ = glGetUniformLocation(program,"chunkZ");

    //Vertex positions:
    //For triangle strip:
    /*const float cube_strip[42] = {
            1.0, 1.0, 0.0, // Back-top-right
            0.0, 1.0, 0.0, // Back-top-left
            1.0, 0.0, 0.0, // Back-bottom-right
            0.0, 0.0, 0.0, // Back-bottom-left
            0.0, 0.0, 1.0, // Front-bottom-left
            0.0, 1.0, 0.0, // Back-top-left
            0.0, 1.0, 1.0, // Front-top-left
            1.0, 1.0, 0.0, // Back-top-right
            1.0, 1.0, 1.0, // Front-top-right
            1.0, 0.0, 0.0, // Back-bottom-right
            1.0, 0.0, 1.0, // Front-bottom-right
            0.0, 0.0, 1.0, // Front-bottom-left
            1.0, 1.0, 1.0, // Front-top-right
            0.0, 1.0, 1.0, // Front-top-left
    };*/


    std::vector<glm::vec3> cubeNormals;
    std::vector<glm::vec3> cubeVerts;
    std::vector<glm::vec2> cubeUVs;

    //Top
    cubeVerts.push_back(glm::vec3(0.0,1.0,0.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,0.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(0.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(0.0,1.0,0.0));

    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(1.0,0.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));

    for(int a = 0; a<6; a++)
        cubeNormals.push_back(glm::vec3(0.0,1.0,0.0));

    //Right
    cubeVerts.push_back(glm::vec3(1.0,0.0,0.0));
    cubeVerts.push_back(glm::vec3(1.0,0.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,0.0));
    cubeVerts.push_back(glm::vec3(1.0,0.0,0.0));

    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));

    for(int a = 0; a<6; a++)
        cubeNormals.push_back(glm::vec3(1.0,0.0,0.0));

    //Left
    cubeVerts.push_back(glm::vec3(0.0, 1.0,   1.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,   1.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,   0.0));
    cubeVerts.push_back(glm::vec3(0.0, 1.0,   1.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,   0.0));
    cubeVerts.push_back(glm::vec3(0.0, 1.0,   0.0));

    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(1.0,0.0));

    for(int a = 0; a<6; a++)
        cubeNormals.push_back(glm::vec3(-1.0,0.0,0.0));

    //Front
    cubeVerts.push_back(glm::vec3(0.0,0.0,1.0));
    cubeVerts.push_back(glm::vec3(0.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,1.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,0.0,1.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,1.0));

    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));

    for(int a = 0; a<6; a++)
        cubeNormals.push_back(glm::vec3(0.0,0.0,1.0));

    //Back
    cubeVerts.push_back(glm::vec3(1.0, 1.0,   0.0));
    cubeVerts.push_back(glm::vec3(0.0, 1.0,  0.0));
    cubeVerts.push_back(glm::vec3(0.0,  0.0,   0.0));
    cubeVerts.push_back(glm::vec3(1.0, 1.0,    0.0));
    cubeVerts.push_back(glm::vec3(0.0, 0.0,  0.0));
    cubeVerts.push_back(glm::vec3(1.0,  0.0,  0.0));

    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(1.0,0.0));

    for(int a = 0; a<6; a++)
        cubeNormals.push_back(glm::vec3(0.0,0.0,-1.0));

    //Bottom
    cubeVerts.push_back(glm::vec3(1.0,0.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,0.0,0.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,0.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,0.0));
    cubeVerts.push_back(glm::vec3(0.0,0.0,1.0));
    cubeVerts.push_back(glm::vec3(1.0,0.0,1.0));

    cubeUVs.push_back(glm::vec2(1.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,0.0));
    cubeUVs.push_back(glm::vec2(0.0,1.0));
    cubeUVs.push_back(glm::vec2(1.0,1.0));

    for(int a = 0; a<6; a++)
        cubeNormals.push_back(glm::vec3(0.0,-1.0,0.0));

    //Create a buffer that just has the above data:
    //Positions:
    glGenBuffers(1,&vertexPositions);
    glBindBuffer(GL_ARRAY_BUFFER,vertexPositions);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3) * cubeVerts.size(),&cubeVerts[0][0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //Normals:
    glGenBuffers(1,&vertexNormals);
    glBindBuffer(GL_ARRAY_BUFFER,vertexNormals);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3) * cubeNormals.size(),&cubeNormals[0][0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //UVs:
    glGenBuffers(1,&vertexUVs);
    glBindBuffer(GL_ARRAY_BUFFER,vertexUVs);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec2) * cubeUVs.size(),&cubeUVs[0][0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //Make sure we only do this once at start-up
    vertexPositionsCreated = true;
}