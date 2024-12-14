#ifndef __ModuleCamera_H__
#define __ModuleCamera_H__

#include "Module.h"
#include "MathGeoLib.h"

class Application;

class ModuleCamera: public Module
{
public:

	ModuleCamera();

	// Destructor
	virtual ~ModuleCamera();

	// Called before quitting
	bool Init();

	update_status Update();

	void RenderCamera();
	void UniformCamera();
	void MoveCamera();
	void OnWindowResize(int width, int height);
	void OrbitCamera();
	void AdaptOnModel(const AABB& aabb);
	void LookAt(const float3& target);

	Frustum frustum;
	float scalefactor = 1.0f;
	int windowScaleX = 1;
	int windowScaleY = 1;
	AABB aabbModel;

private:
	float4x4 model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(math::pi / 4.0f),
		float3(2.0f, 1.0f, 1.0f));
	float4x4 projection = frustum.ProjectionMatrix();
	float4x4 view = frustum.ViewMatrix();
	int* w = new int(0);
	int* h = new int(0);
	float aspect_ratio;
	float focus_offset = 2.0f;

	float speed_base;
	float movement_speed = 0.001f;
	float rotation_speed = 0.0003f;  
	float mouse_rotation_speed = 0.002f;
	float zoom_speed = 0.1f;
	float pan_speed = 0.005f;

	float near_plane_distance = 0.1f;
	float far_plane_distance = 100.0f;

	int frameWidth;
	int frameHeight;
	

};



#endif