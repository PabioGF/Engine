#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

#include "Globals.h"
#include "Model.h"
#include "Mesh.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "MathGeoLib.h"


/**
 * Constructor for the Model class.
 */
Model::Model() {}

/**
 * Destructor for the Model class.
 * Frees all allocated meshes.
 */
Model::~Model() {
    for (Mesh* mesh : meshes) {
        delete mesh;
    }
}

/**
 * Initializes the model.
 *
 * @return true if initialization is successful.
 */
bool Model::Init() {
    return true;
}

/**
 * Updates the model during the application's update loop.
 *
 * @return The update status.
 */
update_status Model::Update() {
    return UPDATE_CONTINUE;
}

/**
 * Loads a GLTF model from a file.
 *
 * @param assetFileName The path to the GLTF file.
 */
void Model::Load(const char* assetFileName) {
    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;
    std::string error, warning;
    int cont = 0;

    if (!gltfContext.LoadASCIIFromFile(&model, &error, &warning, assetFileName)) {
        LOG("Error loading %s: %s", assetFileName, error.c_str());
        Clear();
        return;
    }
    else {
        
        LOG("Model Loaded successfully: %s", assetFileName);

        LoadMaterials(model); 

        for (const auto& srcMesh : model.meshes) {
            for (const auto& primitive : srcMesh.primitives) {
                cont++;
                LOG("Mesh number %d", cont);
                Mesh* mesh = new Mesh;
                mesh->Load(model, srcMesh, primitive);
                mesh->LoadEBO(model, srcMesh, primitive);
                mesh->CreateVAO();
                SaveModelInfo(cont, mesh);
                meshes.push_back(mesh);

            }
        }

        std::string filename(assetFileName);
        if (filename.find("house") != std::string::npos || filename.find("House") != std::string::npos) {  
            App->GetCamera()->scalefactor = 50.0f;
        }
        else {
            App->GetCamera()->scalefactor = 1.0f;
        }
    }
    

}

/**
 * Loads the textures of the model.
 *
 * @param srcModel The GLTF model containing the materials.
 */
void Model::LoadMaterials(const tinygltf::Model& srcModel) {
    for (const auto& srcMaterial : srcModel.materials) {
        unsigned int textureId = 0;

        if (srcMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            const tinygltf::Texture& texture = srcModel.textures[srcMaterial.pbrMetallicRoughness.baseColorTexture.index];
            if (texture.source >= 0 && texture.source < srcModel.images.size()) {
                const tinygltf::Image& image = srcModel.images[texture.source];

                DirectX::ScratchImage scratch_image = App->GetTexture()->LoadTexture("Textures/"+image.uri);
                textureId = App->GetTexture()->CreateTexture(scratch_image);

                scratch_image.Release();
            }
            else {
                LOG("Invalid texture source index: %d", texture.source);
            }
        }
        textures.push_back(textureId);
        SaveTextureInfo(textureId);
    }
}

/**
 * Saves texture information for showing in the UI.
 *
 * @param textureId The texture ID.
 */
void Model::SaveTextureInfo(unsigned int textureId)
{
    int width, height;
    glBindTexture(GL_TEXTURE_2D, textureId);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    textureInfo.push_back("Texture Size: " + std::to_string(width) + "x" + std::to_string(height));

    GLint wrapS, wrapT;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
    textureInfo.push_back("Texture Wrap Mode S: " + std::to_string(wrapS) + ", T: " + std::to_string(wrapT));

    GLint minFilter, magFilter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
    textureInfo.push_back("Texture Min Filter: " + std::to_string(minFilter) + ", Max Filter: " + std::to_string(magFilter));
    glBindTexture(GL_TEXTURE_2D, 0);

    App->GetEditor()->texture_id = textureId;
}

/**
 * Renders all the meshes in the model using the provided shader program.
 *
 * @param program The OpenGL shader program ID.
 */
void Model::RenderModels(unsigned& program) {

    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->Draw(textures, program);
    }

    App->GetEditor()->ModelInformation(modelInfo, textureInfo);
}

/**
 * Sets a new texture for the model.
 *
 * @param textureId The new texture ID.
 */
void Model::SetTexture(unsigned int textureId) {
    textures.clear();
    textureInfo.clear();
    textures.push_back(textureId);
    SaveTextureInfo(textureId);
}

/**
 * Saves information about a mesh for showing in the UI.
 *
 * @param meshIndex The index of the mesh.
 * @param mesh The mesh object.
 */
void Model::SaveModelInfo(const int& meshIndex, Mesh* mesh)
{
    modelInfo.push_back("Mesh Index: " + std::to_string(meshIndex));
    modelInfo.push_back("Vertex number: " + std::to_string(mesh->vertexCount));
    modelInfo.push_back("Index Number: " + std::to_string(mesh->indexCount));

}

/**
 * Calculates the Axis-Aligned Bounding Box (AABB) for the model.
 *
 * @return The calculated AABB.
 */
AABB Model::CalculateAABB() const {
    AABB aabb;
    aabb.SetNegativeInfinity(); 


    for (const Mesh* mesh : meshes) {
        for (size_t i = 0; i < mesh->vertices.size(); i += 3) { 
            float3 vertex = float3(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]);
            if (!aabb.Contains(vertex)) {
                aabb.Enclose(vertex);
            }
        }
    }

    return aabb;
}

/**
 * Clears the model, releasing all allocated resources.
 */
void Model::Clear() {
    modelInfo.clear();
    textureInfo.clear();

    for (Mesh* mesh : meshes) {
        mesh->Clear(); 
        delete mesh;
    }
    meshes.clear();

    for (unsigned texture : textures) {
        glDeleteTextures(1, &texture);
    }
    textures.clear();

    LOG("Model cleared.");
}





