#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "GL/glew.h"

/**
 * Constructor for the ModuleProgram class.
 */
ModuleProgram::ModuleProgram() {

}

/**
 * Destructor for the ModuleProgram class.
 */
ModuleProgram::~ModuleProgram() {

}

/**
 * Loads the source code of a shader from a file.
 *
 * @param shader_file_name The file name of the shader.
 * @return The source code of the shader.
 */
char* ModuleProgram::LoadShaderSource(const char* shader_file_name) {
    char* data = nullptr;
    FILE* file = nullptr;
    fopen_s(&file, shader_file_name, "rb");

    if (file) {
        fseek(file, 0, SEEK_END); 
        int size = ftell(file); 
        data = (char*)malloc(size + 1);
        fseek(file, 0, SEEK_SET); 
        fread(data, 1, size, file);
        data[size] = 0;

        fclose(file);
    }

    return data;
}

/**
 * Compiles a shader from source.
 *
 * @param type The type of the shader.
 * @param source The source code of the shader.
 * @return The shader ID if compilation was successful, otherwise 0.
 */
unsigned ModuleProgram::CompileShader(unsigned type, const char* source)
{
    unsigned shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &source, 0);
    glCompileShader(shader_id);
    int res = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE)
    {
        int len = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int written = 0;
            char* info = (char*)malloc(len);
            glGetShaderInfoLog(shader_id, len, &written, info);
            LOG("Log Info: %s", info);
            free(info);
        }
    }
    return shader_id;
}

/**
 * Creates and links a program from vertex and fragment shaders.
 *
 * @param vtx_shader The ID of the vertex shader.
 * @param frg_shader The ID of the fragment shader.
 * @return The ID of the linked program if successful, otherwise 0.
 */
unsigned ModuleProgram::CreateProgram(unsigned vtx_shader, unsigned frg_shader)
{
    unsigned program_id = glCreateProgram();
    glAttachShader(program_id, vtx_shader);
    glAttachShader(program_id, frg_shader);
    glLinkProgram(program_id);
    int res;
    glGetProgramiv(program_id, GL_LINK_STATUS, &res);
    if (res == GL_FALSE)
    {
        int len = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int written = 0;
            char* info = (char*)malloc(len);
            glGetProgramInfoLog(program_id, len, &written, info);
            LOG("Program Log Info: %s", info);
            free(info);
        }
    }
    glDeleteShader(vtx_shader);
    glDeleteShader(frg_shader);
    return program_id;
}