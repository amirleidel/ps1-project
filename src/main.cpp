#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "fileloader.hpp" // loadFile to string implementation
#include "obj_loader.hpp"


// Load shader code from files
std::string vertexCode = loadFile("shaders/vertex_shader.glsl");
std::string fragmentCode = loadFile("shaders/fragment_shader.glsl");

// Convert to C-style strings for OpenGL
const char* vertexShaderSource = vertexCode.c_str();
const char* fragmentShaderSource = fragmentCode.c_str();

GLuint createShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }
    return shader;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    // Set OpenGL version to 3.3 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    float width = 800, height = 600; // window format
    
    SDL_Window* window = SDL_CreateWindow("OpenGL PSX-Project",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // cam init test 
    // Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(4,4,3), // Camera is at (4,3,3), in World Space xz is horiz y up
        glm::vec3(0,0,0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
    
    
    // array of Meshes
    
    std::vector<Mesh> sceneMeshes;
    
    
    
    // ============ obj import test (1) ============
    Mesh mesh = LoadOBJ("assets/cube-tex.obj");
    
    // set pos argument (frm lvl file later)
    mesh.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
    
    // LoadOBJ already converts to gl texture
    
    // Create VAO and VBO for mesh
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO_positions);
    glGenBuffers(1, &mesh.VBO_texcoords);

    glBindVertexArray(mesh.VAO);

    // --- Positions --- 
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_positions);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.positions.size() * sizeof(glm::vec3),
                 mesh.positions.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // --- Texture coordinates ---
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_texcoords);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.texcoords.size() * sizeof(glm::vec2),
                 mesh.texcoords.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    sceneMeshes.push_back(mesh); // add to list of meshes
    
    // ============ obj import test (2) ============
    mesh = LoadOBJ("assets/cube-tex-colored.obj");
    
    // set pos argument (frm lvl file later)
    mesh.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    
    // LoadOBJ already converts to gl texture
    
    // Create VAO and VBO for mesh
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO_positions);
    glGenBuffers(1, &mesh.VBO_texcoords);

    glBindVertexArray(mesh.VAO);

    // --- Positions --- 
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_positions);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.positions.size() * sizeof(glm::vec3),
                 mesh.positions.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // --- Texture coordinates ---
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_texcoords);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.texcoords.size() * sizeof(glm::vec2),
                 mesh.texcoords.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    sceneMeshes.push_back(mesh); // add to list of meshes
    
    // ================================
    // unbind 
    glBindVertexArray(0);
    
    
    // Compile shaders
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    // link
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    
    // --- Get location of the MVP uniform ---
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    
    glEnable(GL_DEPTH_TEST);
    
    
    // Render loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        
        // Set uniform to use texture unit 0
        GLint texLoc = glGetUniformLocation(shaderProgram, "diffuseTex");
        glUniform1i(texLoc, 0);
        
        for (auto& mesh : sceneMeshes) {
            glm::mat4 mvp = Projection * View * mesh.model; // take view from player object

            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
            glBindTexture(GL_TEXTURE_2D, mesh.diffuseTex);
            glBindVertexArray(mesh.VAO);
            glDrawArrays(GL_TRIANGLES, 0, mesh.positions.size());
        }
        
        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO_positions);
    glDeleteBuffers(1, &mesh.VBO_texcoords);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
