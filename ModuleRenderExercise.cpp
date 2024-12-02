#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleRenderExercise.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"

//https://www.youtube.com/watch?v=e1i_a68CgYE

ModuleRenderExercise::ModuleRenderExercise() {

}

ModuleRenderExercise::~ModuleRenderExercise() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }

    if (shader_program != 0) {
        glDeleteProgram(shader_program);
    }
}

bool ModuleRenderExercise::Init()
{
    bool ret = true;

    vbo = CreateTriangleVBO();

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

    RenderVBO(vbo, shader_program);



    return ret;
}

unsigned ModuleRenderExercise::CreateTriangleVBO() {
    // Vértices del triángulo: posición en 3D (x, y, z)
    float vertices[] = {
        // Primer triángulo
         -1.0,  -1.0f, 0.0f,  // Vértice inferior izquierdo
         1.0f,  -1.0f, 0.0f,  // Vértice inferior derecho
         -1.0f,  1.0f, 0.0f,  // Vértice superior izquierdo

         // Segundo triángulo
          1.0f,  -1.0f, 0.0f,  // Vértice inferior derecho
          1.0f,  1.0f, 0.0f,  // Vértice superior derecho
          -1.0f,  1.0f, 0.0f,   // Vértice superior izquierdo

          0.0f, 1.0f,         //v0 texcoord
          1.0f, 1.0f,         //v1 texcoord
          0.0f, 0.0f,         //v2 texcoord

          1.0f, 1.0f,         //v0 texcoord
          1.0f, 0.0f,         //v1 texcoord
          0.0f, 0.0f         //v2 texcoord
    };


    unsigned vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    return vbo;
}

void ModuleRenderExercise::RenderVBO(unsigned vbo, unsigned program) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 0, "Mi triangulo");
    glUseProgram(shader_program);

    App->GetCamera()->UniformCamera();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,(void*)(sizeof(float)*3*6));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_object);
    glUniform1i(glGetUniformLocation(shader_program, "mytexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);  
    glPopDebugGroup();
}


