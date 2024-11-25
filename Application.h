#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleProgram;
class ModuleDebugDraw;
class ModuleCamera;
class ModuleEditor;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
    void RequestBrowser(const char* url) const;

    ModuleOpenGL* GetOpenGL() { return render; }
    ModuleWindow* GetWindow() { return window; }
    ModuleInput*  GetInput() { return input; }
    ModuleProgram* GetProgram() { return program; }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
    ModuleCamera* GetCamera() { return camera; }
    ModuleEditor* GetEditor() { return editor; }
  
private:

    ModuleOpenGL* render = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleRenderExercise* exercise = nullptr;
    ModuleProgram* program = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleEditor* editor = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
