#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleTexture.h"
using namespace std;

/**
 * Application Constructor
 * Initializes and orders the modules of the engine.
 */
Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(render = new ModuleOpenGL());
	modules.push_back(input = new ModuleInput());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(exercise = new ModuleRenderExercise());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(texture = new ModuleTexture());
	modules.push_back(editor = new ModuleEditor());

}

/**
 * Application Destructor
 * Deletes all the modules in reverse order to ensure proper cleanup.
 */
Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

/**
 * Initializes all modules of the application.
 *
 * @return true if all modules initialized successfully, false otherwise.
 */
bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

/**
 * Updates all modules in the application.
 * Executes the PreUpdate, Update, and PostUpdate cycles for each module.
 *
 * @return the update status of the application (UPDATE_CONTINUE, UPDATE_STOP, or UPDATE_ERROR).
 */
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	if (runningState == UPDATE_STOP) {
		ret =  UPDATE_STOP; 
	}

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

/**
 * Cleans up all modules of the application.
 *
 * @return true if all modules cleaned up successfully, false otherwise.
 */
bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

/**
 * Opens the browser with the specified URL.
 *
 * @param url the URL to open in the browser.
 */
void Application::RequestBrowser(const char* url)const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * Requests to exit the application by setting the running state to UPDATE_STOP.
 */
void Application::RequestExit() {
	runningState = UPDATE_STOP; 
}
