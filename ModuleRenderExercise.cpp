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


/**
 * Constructor for the ModuleRenderExercise class.
 */
ModuleRenderExercise::ModuleRenderExercise() {

}

/**
 * Destructor for the ModuleRenderExercise class.
 */
ModuleRenderExercise::~ModuleRenderExercise() {
    if (shader_program != 0) {
        glDeleteProgram(shader_program);
    }

    if (model != nullptr) {
        model->Clear();
        delete model;
    }
}

/**
 * Initializes the module by loading shaders and models.
 *
 * @return True if initialization is successful, false otherwise.
 */
bool ModuleRenderExercise::Init()
{
    bool ret = true;

    model->Load("Textures/BakerHouse.gltf");
    AABB modelAABB = model->CalculateAABB();
    App->GetCamera()->aabbModel = modelAABB;
    

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

/**
 * Updates the RenderExercise module each frame.
 *
 * @return the current status of the update.
 */
update_status ModuleRenderExercise::Update()
{
    update_status ret = UPDATE_CONTINUE;
    
    model->RenderModels(shader_program);

    return ret;
}

/**
 * Loads a model from a file path when a file is dropped.
 *
 * @param droppedFilePath The path of the file that was dropped.
 */
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

/**
 * Loads a texture from a file path when a file is dropped and sets it on the model.
 *
 * @param droppedFilePath The path of the texture file that was dropped.
 */
void ModuleRenderExercise::LoadDroppedTexture(const char* droppedFilePath) {
    DirectX::ScratchImage scratch_image = App->GetTexture()->LoadTexture(droppedFilePath);
    unsigned int textureId = App->GetTexture()->CreateTexture(scratch_image);
    scratch_image.Release();
    model->SetTexture(textureId);
}

