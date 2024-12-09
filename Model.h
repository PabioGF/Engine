#ifndef __Model_H__
#define __Model_H__

#include "Module.h"
#include "Mesh.h"

namespace tinygltf { // Forward declaration de las clases necesarias
	class Model;
}

class Model
{
public:

	Model();

	// Destructor
	virtual ~Model();

	// Called before quitting
	bool Init();

	update_status Update();
	
	void Load(const char* assetFileName);
	void LoadMaterials(const tinygltf::Model& srcModel);
	void RenderModels(unsigned& program);


private:
	unsigned vao = 0, vbo = 0;
	unsigned shader_program = 0;
	std::vector<unsigned> textures;
	std::vector<Mesh*> meshes;
	//Mesh* new_mesh = nullptr;

public:

};



#endif