//
// Created by Triage on 10/5/2023.
//

#ifndef HALCYONICUS_CHUNK3D_H
#define HALCYONICUS_CHUNK3D_H

#include "../../lib/world/chunk.h"
#include "../../halcyonicus.h"
#include "../Shader.h"

class CHUNK3D {
    GLuint vao;
    GLuint vbo, heightVbo;
    GLuint ebo;
    bool isSetup = false;
    std::vector<GLfloat> cubeVertices = {
            // Front face
            -0.5f, -0.5f,  0.5f, // Vertex 0
            0.5f, -0.5f,  0.5f, // Vertex 1
            0.5f,  0.5f,  0.5f, // Vertex 2
            -0.5f,  0.5f,  0.5f, // Vertex 3

            // Back face
            -0.5f, -0.5f, -0.5f, // Vertex 4
            0.5f, -0.5f, -0.5f, // Vertex 5
            0.5f,  0.5f, -0.5f, // Vertex 6
            -0.5f,  0.5f, -0.5f  // Vertex 7
    };

// Indices to render the cube as a set of triangles
    std::vector<GLuint> cubeIndices = {
            // Front face
            0, 1, 2,
            2, 3, 0,

            // Right face
            1, 5, 6,
            6, 2, 1,

            // Back face
            5, 4, 7,
            7, 6, 5,

            // Left face
            4, 0, 3,
            3, 7, 4,

            // Top face
            3, 2, 6,
            6, 7, 3
    };
public:
    CHUNK * chunk;
    explicit CHUNK3D(CHUNK * c) : chunk{c} {};

    void setup(){

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create and bind VBO for vertex data

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribDivisor(0,0);

        // Upload vertex data to the VBO
        glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(GLfloat), cubeVertices.data(), GL_STATIC_DRAW);

        // Create and bind EBO for indices
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        // Upload index data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), cubeIndices.data(), GL_STATIC_DRAW);

        // Set up vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        // Hopefully make another thing that tells us the location of the blocks, just use a vec3
        glGenBuffers(1, &heightVbo);
        glBindBuffer(GL_ARRAY_BUFFER, heightVbo);
        glVertexAttribDivisor(1,1);
        short* height = &chunk->layers[0].heights[0][0];
        glBufferData(GL_ARRAY_BUFFER, sizeof(chunk->layers[0].heights), height, GL_STATIC_DRAW);
        glVertexAttribIPointer(1, 1, GL_SHORT, 0, nullptr);
        glEnableVertexAttribArray(1);

        // Unbind VAO, VBO, and EBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        isSetup = true;
    }

    void destroy(){
        glDeleteBuffers(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &heightVbo);
        isSetup = false;
    }

    void draw(glm::mat4 view, glm::mat4 projection, Shader* shader){
        if (!isSetup) return;
        glBindVertexArray(vao);
        glm::mat4 model = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setMat4("model", model);
        shader->setVec2("worldOffset", {chunk->location.x, chunk->location.y});

        glDrawElementsInstanced(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, nullptr, 4096);
        glBindVertexArray(0);
    }
};
#endif //HALCYONICUS_CHUNK3D_H
