#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <unordered_map>
#include <glm/glm.hpp> 


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

struct Mesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

struct Texture {
    // TODO: file
};

// Load a simple OBJ file (no materials)
Mesh LoadOBJ(const std::string& path);


struct Model {
    Mesh mesh;
    Texture texture;
};

///

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
    
    /*std::cout << "Loaded OBJ: " << path << "\n";
    std::cout << "  Vertices #: " << mesh.positions.size() << "\n";
    for (int i=0; i < mesh.positions.size(); i++) {
        std::cout << mesh.positions[i] << "\n";}
    std::cout << "  Indices #:  " << mesh.indices.size() << "\n";
    for (int i=0; i < mesh.indices.size(); i++) {
        std::cout << mesh.indices[i] << "\n";}
        */
    return mesh;
}
