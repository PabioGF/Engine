#ifndef __ModuleTexture_H__
#define __ModuleTexture_H__

#include "Module.h"
#include "DirectXTex/DirectXTex/DirectXTex.h"
#include <GL/glew.h>  

class Application;

class ModuleTexture : public Module
{
public:

	ModuleTexture();

	// Destructor
	virtual ~ModuleTexture();

	// Called before quitting
	bool Init();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();
	
	DirectX::ScratchImage LoadTexture(const char* file_path);
	GLuint CreateTexture(const DirectX::ScratchImage& scratch_image);



private:

};



#endif