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


Mesh::Mesh(){

}

Mesh::~Mesh() {

}

void Mesh::Load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive){
    const auto& itPos = primitive.attributes.find("POSITION");
    if (itPos != primitive.attributes.end())//significa que el atributo "POSITION" existe en el primitive.
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];//itPos->second obtiene el índice del accessor asociado al atributo "POSITION"/ model.accessors es un vector que contiene todos los accessors del modelo. / posAcc ahora es una referencia al accessor que contiene la descripción de las posiciones de los vértices.
        SDL_assert(posAcc.type == TINYGLTF_TYPE_VEC3);//Asegura que cada posición de vértice es un vector de 3 componentes (x, y, z).
        SDL_assert(posAcc.componentType == GL_FLOAT);//Asegura que los datos de las posiciones están almacenados como flotantes.
        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView]; //Accede al BufferView asociado al Accessor
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer]; //Acceder al Buffer asociado al BufferView
        const unsigned char* bufferPos = &(posBuffer.data[posAcc.byteOffset + posView.byteOffset]); //Obtiene los datos de posición de los vértices

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
            glBindBuffer(GL_ARRAY_BUFFER, vbo);//NO ESTA EN EL POWER PERO SI LO QUITO PETA.
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
        }
    }

}

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
    }
}



void Mesh::CreateVAO()
{
   // vertexCount = vertices.size() / 3;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * vertexCount));

    glBindVertexArray(0);
}

void Mesh::Draw(const std::vector<unsigned>& textures, unsigned& program)
{

    //LOG("Rendering mesh %d with %d indices and %d vertices", materialIndex, indexCount, vertexCount);
    glUseProgram(program);

    App->GetCamera()->UniformCamera();

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, textures[materialIndex]);
   // glUniform1i(glGetUniformLocation(program, "mytexture"), 0);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

}


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


