#ifndef __ModuleProgram_H__
#define __ModuleProgram_H__

#include "Module.h"

class Application;

class ModuleProgram : public Module
{
public:

	ModuleProgram();

	// Destructor
	virtual ~ModuleProgram();

	char* LoadShaderSource(const char* shader_file_name);
	unsigned CompileShader(unsigned type, const char* source);
	unsigned CreateProgram(unsigned vtx_shader, unsigned frg_shader);

};



#endif