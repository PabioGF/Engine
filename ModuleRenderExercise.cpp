#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleRenderExercise.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleTexture.h"
#include "Model.h"
#include "Mesh.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"

//https://www.youtube.com/watch?v=e1i_a68CgYE

ModuleRenderExercise::ModuleRenderExercise() {

}

ModuleRenderExercise::~ModuleRenderExercise() {
    if (shader_program != 0) {
        glDeleteProgram(shader_program);
    }

    if (model != nullptr) {
        model->Clear();
        delete model;
    }
}

bool ModuleRenderExercise::Init()
{
    bool ret = true;

    //vbo = CreateTriangleVBO();

    model->Load("BakerHouse.gltf");
    AABB modelAABB = model->CalculateAABB();
    App->GetCamera()->aabbModel = modelAABB;
    //LoadDroppedModel("BakerHouse.gltf");
    

    ModuleProgram* program = App->GetProgram();

    const char* vertex_shader = program->LoadShaderSource("vertex_shader.glsl");
    const char* fragment_shader = program->LoadShaderSource("fragment_shader.glsl");

    unsigned vertexShader = program->CompileShader(GL_VERTEX_SHADER, vertex_shader);
    unsigned fragmentShader = program->CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

    shader_program = program->CreateProgram(vertexShader, fragmentShader);

    free((void*)vertex_shader);
    free((void*)fragment_shader);


    return ret;
}

update_status ModuleRenderExercise::Update()
{
    update_status ret = UPDATE_CONTINUE;

    //RenderVBO(vbo, shader_program);
    
    model->RenderModels(shader_program);

    return ret;
}


void ModuleRenderExercise::LoadDroppedModel(const char* droppedFilePath) {
    if (model != nullptr) {
        model->Clear();
        delete model;
    }

    model = new Model();
    model->Load(droppedFilePath);

    AABB modelAABB = model->CalculateAABB();
    App->GetCamera()->AdaptOnModel(modelAABB);
}

void ModuleRenderExercise::LoadDroppedTexture(const char* droppedFilePath) {
    DirectX::ScratchImage scratch_image = App->GetTexture()->LoadTexture(droppedFilePath);
    unsigned int textureId = App->GetTexture()->CreateTexture(scratch_image);
    scratch_image.Release();
    model->SetTexture(textureId);
    LOG("HOLA");
}

