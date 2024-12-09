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

Model::Model() {}

Model::~Model() {
    for (Mesh* mesh : meshes) {
        delete mesh;
    }
}

bool Model::Init() {
    return true;
}

update_status Model::Update() {
    return UPDATE_CONTINUE;
}

void Model::Load(const char* assetFileName) {
    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;
    std::string error, warning;

    if (!gltfContext.LoadASCIIFromFile(&model, &error, &warning, assetFileName)) {
        LOG("Error loading %s: %s", assetFileName, error.c_str());
        return;
    }
    else {
        
        LOG("Model Loaded successfully: %s", assetFileName);

        LoadMaterials(model); 

        for (const auto& srcMesh : model.meshes) {
            for (const auto& primitive : srcMesh.primitives) {
                Mesh* mesh = new Mesh;
                mesh->Load(model, srcMesh, primitive);
                mesh->LoadEBO(model, srcMesh, primitive);
                mesh->CreateVAO();
                meshes.push_back(mesh);

            }
        }
    }
    

}

void Model::LoadMaterials(const tinygltf::Model& srcModel) {
    for (const auto& srcMaterial : srcModel.materials) {
        unsigned int textureId = 0;

        if (srcMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            const tinygltf::Texture& texture = srcModel.textures[srcMaterial.pbrMetallicRoughness.baseColorTexture.index];
            if (texture.source >= 0 && texture.source < srcModel.images.size()) {
                const tinygltf::Image& image = srcModel.images[texture.source];
                //LOG("Loading texture: %s", image.uri.c_str());

                DirectX::ScratchImage scratch_image = App->GetTexture()->LoadTexture(image.uri);
                textureId = App->GetTexture()->CreateTexture(scratch_image);
            }
            else {
                LOG("Invalid texture source index: %d", texture.source);
            }
        }

        textures.push_back(textureId);
    }
}

void Model::RenderModels(unsigned& program) {
    LOG("MESEHS: %d", meshes.size());
    for (Mesh* mesh : meshes) {
        mesh->Draw(textures, program);
    }
}

