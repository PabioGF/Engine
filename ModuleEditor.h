#ifndef __ModuleEditor_H__
#define __ModuleEditor_H__

#include "Module.h"
#include <vector>
#include <string>

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

private: 
	bool show_about_window = false;
	int fps_counter = 0;
	float delta_time;
	int frames = 0;
	double starttime = 0;
	bool first = TRUE;
	float fps = 0.0f;

	std::vector<std::string> logBuffer;
	std::vector<float> fps_log; 
	std::vector<float> ms_log;  
	const size_t log_size = 100;
	bool showcase = false;
	bool fullscreen = false;
	bool resizable = false;
};



#endif