#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "SDL.h"
#include "GL/glew.h"
#include <fstream>
#include <string>
#define PROGRAM_NAME "Pablo's Engine"
#define LIBRARIES "SDL 2.0\nOpenGL 4.6\nMathGeoLib 1.5\nImGui 1.89"

const char version = 2.0;

/**
 * Constructor for the ModuleOpenGL class.
 */
ModuleOpenGL::ModuleOpenGL()
{
}

/**
 * Destructor for the ModuleOpenGL class.
 */
ModuleOpenGL::~ModuleOpenGL()
{
}


/**
 * Initializes the OpenGL module.
 *
 * @return true if initialization was successful.
 */
bool ModuleOpenGL::Init()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	context = SDL_GL_CreateContext(App->GetWindow()->window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

	}

	LOG("Using Glew %s", glewGetString(version));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	glFrontFace(GL_CCW); // Front faces will be counter clockwise

	program_name = PROGRAM_NAME;
	libraries = LIBRARIES;
	license_text = LoadLicense("LICENCE.txt");
	


	LOG("Creating Renderer context");

	return true;
}

/**
 * Prepares for updates in each frame.
 *
 * @return UPDATE_CONTINUE to continue updating.
 */
update_status ModuleOpenGL::PreUpdate()
{
	int w = 0;
	int h = 0;

	SDL_GetWindowSize(App->GetWindow()->window, &w, &h);
	glViewport(0, 0, w, h);

	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

/**
 * Updates the OpenGL module each frame.
 *
 * @return the current status of the update.
 */
update_status ModuleOpenGL::Update()
{
	return UPDATE_CONTINUE;
}

/**
 * Swaps the OpenGL window.
 *
 * @return the current status of the update.
 */
update_status ModuleOpenGL::PostUpdate()
{
	SDL_GL_SwapWindow(App->GetWindow()->window);
	return UPDATE_CONTINUE;
}

/**
 * Cleans up the OpenGL module.
 *
 * @return true if cleanup was successful.
 */
bool ModuleOpenGL::CleanUp()
{
	LOG("Destroying renderer");
	SDL_GL_DeleteContext(context);

	return true;
}

/**
 * Handles window resizing.
 *
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
void ModuleOpenGL::WindowResized(unsigned width, unsigned height)
{
	glViewport(0, 0, width, height);
	App->GetCamera()->OnWindowResize(width, height);
}

/**
 * Enables or disables depth test and cull face in OpenGL.
 *
 * @param depth_test The state of the depth test.
 * @param cull_face The state of the cull face.
 */
void ModuleOpenGL::EnableParameters(bool& depth_test, bool& cull_face) {
	if (depth_test) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	if (cull_face) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
	
	
}

/**
 * Loads the license text from a file.
 *
 * @param file_path The file path to the license file.
 * @return The content of the license file as a string.
 */
std::string ModuleOpenGL::LoadLicense(const std::string& file_path)
{
	std::ifstream file(file_path);
	if (!file.is_open())
	{
		LOG("Error: Could not open license file at %s", file_path.c_str());
		return "License file not found.";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

