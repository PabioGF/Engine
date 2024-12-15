#ifndef __Model_H__
#define __Model_H__

#include "Module.h"
#include "Mesh.h"
#include "MathGeoLib.h"

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
	void SaveModelInfo(const int& meshIndex, Mesh* mesh);
	void SaveTextureInfo(unsigned int textureId);
	void SetTexture(unsigned int textureId);
	AABB CalculateAABB() const;
	void Clear();

private:
	std::vector<unsigned> textures;
	std::vector<Mesh*> meshes;
	std::vector<std::string> modelInfo;
	std::vector<std::string> textureInfo;

public:

};



#endif