#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <unordered_map>
#include <glm/glm.hpp> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// vec3 printing
std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// vec2 printing
std::ostream& operator<<(std::ostream& os, const glm::vec2& v)
{
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

struct Material {
    std::string name;

    glm::vec3 Ka = glm::vec3(0.0f);  // Ambient col
    glm::vec3 Kd = glm::vec3(1.0f);  // Diffuse col
    glm::vec3 Ks = glm::vec3(0.0f);  // Specular col
    float Ns = 0.0f;                 // Shine
    float d = 1.0f;                  // Transparency
    int illum = 2;                   // illum model

    std::string diffuseTexPath;
    std::string normalMapPath;
    std::string specularMapPath;

    unsigned int diffuseTexID = 0;
    unsigned int normalMapID = 0;
    unsigned int specularTexID = 0;
};

// Mesh with associated material and texture
struct Mesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    
    std::string materialLib;    // Path to .mtl
    std::string activeMaterial; // active material
    
    Material material;
    
    // OpenGL texture
    GLuint diffuseTex = 0;   
    GLuint specularTex = 0;
    GLuint normalTex = 0;
};

// Load a simple OBJ file (no materials)
Mesh LoadOBJ(const std::string& path);
// Load mtl file
std::vector<Material> LoadMTL(const std::string& path);


///

// TODO: Atm multiple mats as vect, but not necessary
std::vector<Material> LoadMTL(const std::string& path)
{
    std::vector<Material> materials;
    Material current;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open MTL file: " << path << "\n";
        return materials;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Strip comments
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos)
            line = line.substr(0, comment_pos);

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "newmtl") {
            // Start new material
            if (!current.name.empty())
                materials.push_back(current);
            current = Material{};
            ss >> current.name;
        }
        else if (cmd == "Ka") ss >> current.Ka.r >> current.Ka.g >> current.Ka.b;
        else if (cmd == "Kd") ss >> current.Kd.r >> current.Kd.g >> current.Kd.b;
        else if (cmd == "Ks") ss >> current.Ks.r >> current.Ks.g >> current.Ks.b;
        else if (cmd == "Ns") ss >> current.Ns;
        else if (cmd == "d" || cmd == "Tr") ss >> current.d;  // Transparency
        else if (cmd == "illum") ss >> current.illum;
        else if (cmd == "map_Kd") ss >> current.diffuseTexPath;
        else if (cmd == "map_Ks") ss >> current.specularMapPath;
        else if (cmd == "bump" || cmd == "map_Bump") ss >> current.normalMapPath;
    }

    // Push last material
    if (!current.name.empty())
        materials.push_back(current);
    /*
    std::cout << "Loaded " << materials.size() << " material(s) from " << path << "\n";
    for (const auto& m : materials)
    {
        std::cout << "  Material: " << m.name << "\n";
        std::cout << "    Kd: " << m.Kd.r << ", " << m.Kd.g << ", " << m.Kd.b << "\n";
        if (!m.diffuseTexPath.empty())
            std::cout << "    Diffuse map: " << m.diffuseTexPath << "\n";
        if (!m.normalMapPath.empty())
            std::cout << "    Normal map: " << m.normalMapPath << "\n";
    }
    */
    return materials;
}

GLuint LoadTextureFromFile(const std::string& baseDir, const std::string& fileName)
{
    std::string fullPath = baseDir + fileName;

    int width, height, channels;
    // Load image with stb_image
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &channels, 4); // force 4 channels RGBA
    if (!data) {
        std::cerr << "Failed to load texture: " << fullPath << "\n";
        return 0;  // 0 is not a valid OpenGL texture
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload the texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Optional: generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    return textureID;
}

Mesh LoadOBJ(const std::string& path)
{
    Mesh mesh;

    // Temporary storage for data before faces are processed
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texcoords;
    std::vector<glm::vec3> temp_normals;

    std::ifstream file(path);
    if (!file) {
        std::cerr << "Error: Cannot open OBJ file: " << path << "\n";
        return mesh;
    }

    std::string line;
    
    
    while (std::getline(file, line))
    {
        
        
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos)
            line = line.substr(0, comment_pos);

        auto is_not_space = [](unsigned char c) { return !std::isspace(c); };
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), is_not_space));
        line.erase(std::find_if(line.rbegin(), line.rend(), is_not_space).base(), line.end());

        // Skip empty or comment-only lines
        if (line.empty())
            continue;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "v") {
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;
            temp_positions.push_back(v);
        }
        else if (cmd == "vt") {
            glm::vec2 vt;
            ss >> vt.x >> vt.y;
            temp_texcoords.push_back(vt);
        }
        else if (cmd == "vn") {
            glm::vec3 vn;
            ss >> vn.x >> vn.y >> vn.z;
            temp_normals.push_back(vn);
        }
        else if (cmd == "mtllib") {
            ss >> mesh.materialLib;
        }
        else if (cmd == "usemtl") {
            ss >> mesh.activeMaterial;
        }
        else if (cmd == "f") {
            std::string vertex_str;
            while (ss >> vertex_str) {
                unsigned int vi = 0, ti = 0, ni = 0;

                size_t firstSlash = vertex_str.find('/');
                size_t secondSlash = vertex_str.find('/', firstSlash + 1);

                if (firstSlash == std::string::npos) {
                    // format: f v
                    vi = std::stoi(vertex_str);
                } else if (secondSlash == std::string::npos) {
                    // format: f v/t
                    vi = std::stoi(vertex_str.substr(0, firstSlash));
                    ti = std::stoi(vertex_str.substr(firstSlash + 1));
                } else {
                    // format: f v/t/n or f v//n
                    vi = std::stoi(vertex_str.substr(0, firstSlash));
                    if (secondSlash > firstSlash + 1)
                        ti = std::stoi(vertex_str.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                    if (secondSlash + 1 < vertex_str.size())
                        ni = std::stoi(vertex_str.substr(secondSlash + 1));
                }

                if (vi > 0 && vi <= temp_positions.size())
                    mesh.positions.push_back(temp_positions[vi - 1]);
                if (ti > 0 && ti <= temp_texcoords.size())
                    mesh.texcoords.push_back(temp_texcoords[ti - 1]);
                if (ni > 0 && ni <= temp_normals.size())
                    mesh.normals.push_back(temp_normals[ni - 1]);

                mesh.indices.push_back(static_cast<unsigned int>(mesh.indices.size()));
            }
        }
    }
    
    // Add material to mesh
    if (!mesh.materialLib.empty()) {
        std::string mtlPath = "assets/" + mesh.materialLib;
        auto materials = LoadMTL(mtlPath);
        
        for (auto& mat : materials) {
            if (mat.name == mesh.activeMaterial) {
                mesh.material = mat; 
                break;
            }
        }
        
    // Load diffuse texture from file
    // using mesh.material.diffuseTexPath;
    /* load an image file directly as a new OpenGL texture */
    
    std::cout << mesh.material.diffuseTexPath << "\n";
    mesh.diffuseTex = LoadTextureFromFile("assets/", mesh.material.diffuseTexPath);
        
    /* check for an error during the load process */
    if (mesh.diffuseTex == 0) {
        std::cerr << "Failed to load diffuse texture!\n";
    } else {
        std::cout << "Diffuse texture loaded successfully, ID: " << mesh.diffuseTex << "\n";
    }
    
    if (glIsTexture(mesh.diffuseTex)) {
        std::cout << "Diffuse texture is valid!\n";
    } else {
        std::cerr << "Diffuse texture is not valid!\n";
    }

    }
    /*
    std::cout << "Loaded OBJ: " << path << "\n";
    std::cout << "  Vertices #: " << mesh.positions.size() << "\n";
    for (int i=0; i < mesh.positions.size(); i++) {
        std::cout << mesh.positions[i] << "\n";}
    std::cout << "  Indices #:  " << mesh.indices.size() << "\n";
    for (int i=0; i < mesh.indices.size(); i++) {
        std::cout << mesh.indices[i] << "\n";}
    
    std::cout << mesh.materialLib << "\n";
    std::cout << mesh.activeMaterial << "\n";
    */
    return mesh;
}
