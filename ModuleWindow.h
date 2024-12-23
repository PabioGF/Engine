#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	void SetFullScreen(bool fullscreen);
	void SetResizable(bool resizable);

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

};

#endif // __ModuleWindow_H__