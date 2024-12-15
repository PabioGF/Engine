#pragma once
#include "Module.h"
#include "Globals.h"
#include <string>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleOpenGL : public Module
{
public:
	ModuleOpenGL();
	~ModuleOpenGL();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void EnableParameters(bool& depth_test, bool& cull_face);
	std::string LoadLicense(const std::string& file_path);
	void* context;
	const char* program_name;
	const char* libraries;
	std::string license_text;
public:
	bool depth_test;
	bool cull_face;
	
};
