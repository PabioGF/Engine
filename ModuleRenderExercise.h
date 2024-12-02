#ifndef __ModuleRenderExercise_H__
#define __ModuleRenderExercise_H__

#include "Module.h"
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

	unsigned CreateTriangleVBO();
	void RenderVBO(unsigned vbo, unsigned program);


private:
	unsigned vao = 0, vbo = 0;
	unsigned shader_program = 0;

public:
	GLuint texture_object;
	
};



#endif