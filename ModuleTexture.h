#ifndef __ModuleTexture_H__
#define __ModuleTexture_H__

#include "Module.h"
#include "DirectXTex/DirectXTex/DirectXTex.h"
#include <GL/glew.h>  
#include <string>

class Application;

class ModuleTexture : public Module
{
public:

	ModuleTexture();

	// Destructor
	virtual ~ModuleTexture();
	
	DirectX::ScratchImage LoadTexture(const std::string& file_path);
	unsigned int CreateTexture(const DirectX::ScratchImage& scratch_image);



private:

};



#endif