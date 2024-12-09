#ifndef MESH_H
#define MESH_H

#include <vector>
#include "GL/glew.h"

namespace tinygltf { // Forward declaration de las clases necesarias
    class Model;
    struct Mesh;
    struct Primitive;
}

class Mesh {
public:
    unsigned vbo;
    unsigned ebo;
    unsigned vao;
    std::vector<float> vertices; 
    std::vector<unsigned int> indices; 
    int materialIndex;    

    Mesh();
    ~Mesh();

    void Load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
    void Render(unsigned& program);  
    void LoadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
    void CreateVAO();
    void Draw(const std::vector<unsigned>& textures, unsigned& program);

private:
    unsigned int vertexCount;
    unsigned int indexCount;
    unsigned int uvsCount;
};

#endif 