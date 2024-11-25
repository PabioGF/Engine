#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL/include/SDL_scancode.h"
#include "SDL/include/SDL_events.h"

typedef unsigned __int8 Uint8;

class ModuleInput : public Module
{
public:
    ModuleInput();
    ~ModuleInput();

    bool Init();
    update_status Update();
    bool CleanUp();

    bool IsKeyPressed(SDL_Scancode key) const;

    bool IsMouseButtonPressed(Uint8 button) const;

    int GetMouseMotionX() const { return mouseMotionX; }
    int GetMouseMotionY() const { return mouseMotionY; }

    SDL_Event sdlEvent;

private:
    const Uint8* keyboard = NULL;
    Uint32 mouseState = 0;
    int mouseMotionX = 0; 
    int mouseMotionY = 0; 
};