#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__

#include "Module.h"
#include <vector>
#include <string>
#include <chrono>

class Application;

class ModuleEditor : public Module
{
public:

	ModuleEditor();

	// Destructor
	virtual ~ModuleEditor();

	// Called before quitting
	bool Init();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	void AddLog(const char* log,...);
	void FpsGraph();
	void ModelInformation(const std::vector<std::string>& modelInfo, const std::vector<std::string>& textureInfo);
	void ShowSystemInfoWindow();
	void GetMemoryInfo(size_t& total_memory, size_t& used_memory);
	void ConfigurationsMenu();

	unsigned int texture_id = 0;

private: 
	bool showWindows;
	bool show_about_window = false;
	int fps_counter = 0;
	float delta_time;

	std::vector<float> fps_log; 
	std::vector<float> ms_log;  
	const size_t log_size = 100;
	bool showcase = false;
	bool fullscreen = false;
	bool resizable = false;
	bool depth_test = true;
	bool cull_face = true;
};



#endif