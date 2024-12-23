#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleRenderExercise.h"
#include "ModuleTexture.h"
#include "SDL/include/SDL.h"

/**
 * Constructor for the ModuleInput class.
 */
ModuleInput::ModuleInput()
{}

/**
 * Destructor for the ModuleInput class.
 */
ModuleInput::~ModuleInput()
{}

/**
 * Initializes the Input Module.
 *
 * @return true if initialization was successful.
 */
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

/**
 * Updates the input module each frame.
 *
 * @return The current update status.
 */
update_status ModuleInput::Update()
{
    mouseMotionX = 0;
    mouseMotionY = 0;
    mouseWheel = 0;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;
            case SDL_WINDOWEVENT:
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    App->GetOpenGL()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                break;
            case SDL_MOUSEMOTION:
                mouseMotionX = sdlEvent.motion.xrel; 
                mouseMotionY = sdlEvent.motion.yrel; 
                break;
            case SDL_MOUSEWHEEL:
                mouseWheel = sdlEvent.wheel.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouseState = SDL_GetMouseState(nullptr, nullptr); 
                break;
            case SDL_DROPFILE:
                //WE CHECK IF IS A MODEL OR A TEXTURE
                char* droppedFilePath = sdlEvent.drop.file;
                std::string filePath(droppedFilePath);
                LOG("File dropped: %s", droppedFilePath);

                if (filePath.find(".gltf") != std::string::npos || filePath.find(".fbx") != std::string::npos) {
                    App->GetExercise()->LoadDroppedModel(droppedFilePath);
                }
                else if (filePath.find(".png") != std::string::npos ||
                    filePath.find(".jpg") != std::string::npos ||
                    filePath.find(".dds") != std::string::npos) {
                    App->GetExercise()->LoadDroppedTexture(droppedFilePath);
                }
                else {
                    LOG("Unsupported file format: %s", droppedFilePath);
                }


                SDL_free(droppedFilePath);

        }
    }

    keyboard = SDL_GetKeyboardState(NULL);

    return UPDATE_CONTINUE;
}

/**
 * Checks if a specific key is pressed.
 *
 * @param key The SDL_Scancode of the key.
 * @return true if the key is pressed, false otherwise.
 */
bool ModuleInput::IsKeyPressed(SDL_Scancode key) const
{
    return keyboard[key] != 0;
}

/**
 * Cleans the Input Module
 *
 * @return true if cleanup was successful.
 */
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

/**
 * Checks if a specific mouse button is pressed.
 *
 * @param button The SDL button identifier.
 * @return true if the mouse button is pressed, false otherwise.
 */
bool ModuleInput::IsMouseButtonPressed(Uint8 button) const
{
    return (mouseState & SDL_BUTTON(button)) != 0;
}


