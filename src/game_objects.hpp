#include <iostream>
#include <string>
#include <vector>

//#include "obj_loader.hpp"

struct GameObject {
    glm::vec3 position = glm::vec3(0.0f);
    
    Mesh mesh;
    bool visible = true;
    
    // add changepos here or in render loop
    
    void addMesh(Mesh mesh) {
        
        this->mesh = mesh;
        
        // set pos argument frm lvl file
        this->mesh.model = glm::translate(glm::mat4(1.0f), this->position);
    }
        
    void sendMesh() {
        // Create VAO and VBO for mesh
        glGenVertexArrays(1, &this->mesh.VAO);
        glGenBuffers(1, &this->mesh.VBO_positions);
        glGenBuffers(1, &this->mesh.VBO_texcoords);

        glBindVertexArray(this->mesh.VAO);

        // --- Positions --- 
        glBindBuffer(GL_ARRAY_BUFFER, this->mesh.VBO_positions);
        glBufferData(GL_ARRAY_BUFFER,
                     this->mesh.positions.size() * sizeof(glm::vec3),
                     this->mesh.positions.data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // --- Texture coordinates ---
        glBindBuffer(GL_ARRAY_BUFFER, this->mesh.VBO_texcoords);
        glBufferData(GL_ARRAY_BUFFER,
                     this->mesh.texcoords.size() * sizeof(glm::vec2),
                     this->mesh.texcoords.data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);
    }
        
        

};