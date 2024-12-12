#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleRenderExercise.h"
#include "SDL/include/SDL.h"

ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
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

// Called every draw update
update_status ModuleInput::Update()
{
    

    mouseMotionX = 0;
    mouseMotionY = 0;

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
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouseState = SDL_GetMouseState(nullptr, nullptr); // Update mouse button state
                break;
            case SDL_DROPFILE:
                char* droppedFilePath = sdlEvent.drop.file;

                LOG("File dropped: %s", droppedFilePath);

                App->GetExercise()->LoadDroppedModel(droppedFilePath);

                SDL_free(droppedFilePath);

        }
    }

    keyboard = SDL_GetKeyboardState(NULL);

    return UPDATE_CONTINUE;
}

bool ModuleInput::IsKeyPressed(SDL_Scancode key) const
{
    return keyboard[key] != 0;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool ModuleInput::IsMouseButtonPressed(Uint8 button) const
{
    return (mouseState & SDL_BUTTON(button)) != 0;
}
