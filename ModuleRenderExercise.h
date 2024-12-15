#ifndef __ModuleRenderExercise_H__
#define __ModuleRenderExercise_H__

#include "Module.h"
#include "Model.h"
#include <GL/glew.h>  

class Application;

class ModuleRenderExercise : public Module
{
public:

	ModuleRenderExercise();

	// Destructor
	virtual ~ModuleRenderExercise();

	// Called before quitting
	bool Init();

	update_status Update();

	void LoadDroppedModel(const char* droppedFilePath);
	void LoadDroppedTexture(const char* droppedFilePath);


private:
	unsigned shader_program = 0;
	Model* model = new Model();

	
};



#endif