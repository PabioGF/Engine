#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
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
    unsigned int vertexCount;
    unsigned int indexCount;

    Mesh();
    ~Mesh();

    void Load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
    void Render(unsigned& program);  
    void LoadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
    void CreateVAO();
    void SetTexture(unsigned int textureId);
    void Draw(const std::vector<unsigned>& textures, unsigned& program);
    void Clear();

private:

    unsigned int uvsCount;
    
   
};

#endif 