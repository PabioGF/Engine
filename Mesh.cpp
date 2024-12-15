#include "Mesh.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "Globals.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "ModuleEditor.h"
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "tiny_gltf.h"

/**
 * Constructor for the Mesh class.
 */
Mesh::Mesh(){

}

/**
 * Destructor for the Mesh class.
 */
Mesh::~Mesh() {

}

/**
 * Loads vertex and texture coordinate data into the mesh.
 *
 * @param model Reference to the GLTF model.
 * @param mesh The mesh data from the GLTF file.
 * @param primitive The primitive data within the mesh.
 */
void Mesh::Load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive){
    const auto& itPos = primitive.attributes.find("POSITION");
    if (itPos != primitive.attributes.end())
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
        SDL_assert(posAcc.type == TINYGLTF_TYPE_VEC3);
        SDL_assert(posAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView]; 
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const unsigned char* bufferPos = &(posBuffer.data[posAcc.byteOffset + posView.byteOffset]); 

        const auto& itTex = primitive.attributes.find("TEXCOORD_0");
        if (itTex != primitive.attributes.end())
        {
            if (primitive.material >= 0 && primitive.material < model.materials.size()) {
                materialIndex = primitive.material;
            }
            else {
                materialIndex = -1;
                LOG("Primitive does not have a valid material.");
            }
            const tinygltf::Accessor& uvsAcc = model.accessors[itTex->second];
            SDL_assert(uvsAcc.type == TINYGLTF_TYPE_VEC2);
            SDL_assert(uvsAcc.componentType == GL_FLOAT);
            uvsCount = uvsAcc.count;
            const tinygltf::BufferView& uvsView = model.bufferViews[uvsAcc.bufferView];
            const tinygltf::Buffer& uvsBuffer = model.buffers[uvsView.buffer];
            const unsigned char* bufferUvs = &(uvsBuffer.data[uvsAcc.byteOffset + uvsView.byteOffset]);


            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, ((sizeof(float) * 3 * posAcc.count) + (sizeof(float) * 2 * uvsAcc.count)), nullptr, GL_STATIC_DRAW);
            float3 * ptr = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            for (size_t i = 0; i < posAcc.count; ++i)
            {
                float3 vertex = *reinterpret_cast<const float3*>(bufferPos);
                ptr[i] = vertex;

                vertices.push_back(vertex.x);
                vertices.push_back(vertex.y);
                vertices.push_back(vertex.z);

                if(posView.byteStride == 0){
                    bufferPos += sizeof(float3);
                }else{
                    bufferPos += posView.byteStride;
                }
                

            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
            float2* ptr2 = reinterpret_cast<float2*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            int offset = (posAcc.count * 3) / 2;
            for (size_t i = offset; i < offset + uvsAcc.count; ++i)
            {
                ptr2[i] = *reinterpret_cast<const float2*>(bufferUvs);
                if (uvsView.byteStride == 0) {
                    bufferUvs += sizeof(float2);
                }
                else {
                    bufferUvs += uvsView.byteStride;
                }
                

            }
            glUnmapBuffer(GL_ARRAY_BUFFER);



            vertexCount = posAcc.count;

            LOG("VBO loaded");
        }

    }

}

/**
 * Loads index data into the Element Buffer Object (EBO).
 *
 * @param model Reference to the GLTF model.
 * @param mesh The mesh data from the GLTF file.
 * @param primitive The primitive data within the mesh.
 */
void Mesh::LoadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indAcc.count, nullptr, GL_STATIC_DRAW);
        unsigned int* ptr = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

        const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
        const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset +
            indView.byteOffset]);

        switch (indAcc.componentType) {
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
            for (size_t i = 0; i < indAcc.count; ++i) {
                ptr[i] = bufferInd[i];
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
            for (size_t i = 0; i < indAcc.count; ++i) {
                ptr[i] = bufferInd[i];
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
            for (size_t i = 0; i < indAcc.count; ++i) {
                ptr[i] = bufferInd[i];
                
            }
            break;
        }
        default: {
            assert(false && "Unsupported index type");
            break;
        }
        }
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        indexCount = indAcc.count;

        LOG("EBO loaded");
    }
}


/**
 * Creates the Vertex Array Object (VAO).
 */
void Mesh::CreateVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * vertexCount));

    glBindVertexArray(0);

    LOG("VAO created");
}

/**
 * Draws the mesh.
 *
 * @param textures The textures associated with the mesh.
 * @param program The shader program to use.
 */
void Mesh::Draw(const std::vector<unsigned>& textures, unsigned& program)
{

    glUseProgram(program);

    App->GetCamera()->UniformCamera();

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, textures[materialIndex]);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

}

/**
 * Clears the mesh data.
 */
void Mesh::Clear() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (ebo != 0) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    LOG("Mesh cleared.");
}


