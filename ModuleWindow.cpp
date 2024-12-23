#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

/**
 * Constructor for the ModuleWindow class.
 */
ModuleWindow::ModuleWindow()
{
}

/**
 * Destructor for the ModuleWindow class.
 */
ModuleWindow::~ModuleWindow()
{
}

/**
 * Initializes the SDL window and surface.
 *
 * @return True if initialization is successful, false otherwise.
 */
bool ModuleWindow::Init()
{
	SDL_WINDOW_OPENGL;
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;

	}
	else
	{
		SDL_DisplayMode desktopMode;
		if (SDL_GetDesktopDisplayMode(0, &desktopMode) != 0) {
			LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
			return false;
		}

		//Create window
		width = desktopMode.w;
		height = desktopMode.h;
		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;

		if(FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

/**
 * Cleans up the SDL window by destroying it and quitting all SDL systems.
 *
 * @return True if cleanup is successful, false otherwise.
 */
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

/**
 * Sets the window to fullscreen mode.
 *
 * @param fullscreen True to enable fullscreen, false to disable it.
 */
void ModuleWindow::SetFullScreen(bool fullscreen) {
	LOG("FULL SCREEN");
	if (fullscreen)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		SDL_SetWindowFullscreen(window, 0);
	}
}

/**
 * Sets the window to be resizable.
 *
 * @param resizable True to make the window resizable, false otherwise.
 */
void ModuleWindow::SetResizable(bool resizable) {
	LOG("RESIZABLE SCREEN");
	if (resizable)
	{
		SDL_SetWindowResizable(window, SDL_TRUE);
	}
	else
	{
		SDL_SetWindowResizable(window, SDL_FALSE);
	}
}

