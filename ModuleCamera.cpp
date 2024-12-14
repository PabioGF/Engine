#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleDebugDraw.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"

//https://www.youtube.com/watch?v=e1i_a68CgYE

ModuleCamera::ModuleCamera() {
    frustum.type = FrustumType::PerspectiveFrustum;

    frustum.pos = float3(0.0f, 0.0f, 15.0f);
    frustum.front = -float3::unitZ;
    frustum.up = float3::unitY;

    speed_base = movement_speed;


   // SDL_GetWindowSize(App->GetWindow()->window, w, h);


}

ModuleCamera::~ModuleCamera() {
    if (w != nullptr) {
        delete w;
    }

    if (h != nullptr) {
        delete h;
    }

   

    App->GetDebugDraw()->CleanUp();
}

bool ModuleCamera::Init()
{
    bool ret = true;
    OnWindowResize(App->GetWindow()->width, App->GetWindow()->height);
    return ret;
}

update_status ModuleCamera::Update()
{
    update_status ret = UPDATE_CONTINUE;

    RenderCamera();
    MoveCamera();
   // LOG("frustrum pos: %d", frustum.pos);
    return ret;
}



void ModuleCamera::RenderCamera() {

    

    model = float4x4::FromTRS(float3(0.0f, 0.0f, 0.0f),
        float4x4::RotateZ(0),
        float3(1.0f, 1.0f, 1.0f)*scalefactor);

    SDL_GetWindowSize(App->GetWindow()->window, w, h);

    frustum.nearPlaneDistance = near_plane_distance;
    frustum.farPlaneDistance = far_plane_distance;
    frustum.verticalFov = math::pi / 4.0f;
    frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);

    projection = frustum.ProjectionMatrix();
    view = frustum.ViewMatrix();

    
    App->GetDebugDraw()->Draw(view, projection, *w, *h);


}

void ModuleCamera::UniformCamera() {
    glUniformMatrix4fv(0, 1, GL_TRUE, &projection[0][0]);
    glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
    glUniformMatrix4fv(2, 1, GL_TRUE, &model[0][0]);
}

void ModuleCamera::MoveCamera() {

    
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_LSHIFT))
    {
        movement_speed = speed_base * 3;
    }
    else {
        movement_speed = speed_base;
    }

    //TRANSLATION
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_W))
    {
        frustum.pos += frustum.front * movement_speed;
    }
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_S))
    {
        frustum.pos -= frustum.front * movement_speed;
    }

    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_A))
    {
        frustum.pos -= frustum.WorldRight() * movement_speed;
    }
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_D))
    {
        frustum.pos += frustum.WorldRight() * movement_speed;
    }

    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_E))
    {
        frustum.pos += frustum.up * movement_speed;
    }
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_Q))
    {
        frustum.pos -= frustum.up * movement_speed;
    }

    //ROTATION
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_UP) && frustum.front.y < 0.9f)
    {
        LOG("Pitch up");
        Quat pitchRotation = Quat::RotateAxisAngle(frustum.WorldRight(), rotation_speed);
        frustum.front = pitchRotation.Mul(frustum.front).Normalized();
        frustum.up = pitchRotation.Mul(frustum.up).Normalized();
    }
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_DOWN) && frustum.front.y > -0.9f)
    {
        LOG("Pitch down");
        Quat pitchRotation = Quat::RotateAxisAngle(frustum.WorldRight(), -rotation_speed);
        frustum.front = pitchRotation.Mul(frustum.front).Normalized();
        frustum.up = pitchRotation.Mul(frustum.up).Normalized();
    }

    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_LEFT))
    {
        LOG("Yaw left");
        Quat yawRotation = Quat::RotateY(rotation_speed);
        frustum.front = yawRotation.Mul(frustum.front).Normalized();
        frustum.up = yawRotation.Mul(frustum.up).Normalized();
    }
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_RIGHT))
    {
        LOG("Yaw right");
        Quat yawRotation = Quat::RotateY(-rotation_speed);
        frustum.front = yawRotation.Mul(frustum.front).Normalized();
        frustum.up = yawRotation.Mul(frustum.up).Normalized();
    }

    
    if (App->GetInput()->IsMouseButtonPressed(SDL_BUTTON_RIGHT))
    {
        //ZOOMING
        if(App->GetInput()->IsKeyPressed(SDL_SCANCODE_LALT)){
            int mouseY = App->GetInput()->GetMouseMotionY();

            if (mouseY != 0)
            {
                frustum.pos += frustum.front * mouseY * zoom_speed; 
            }
        
        }
        else {
            // ROTATION (Mouse motion)
            int mouseX = App->GetInput()->GetMouseMotionX();
            int mouseY = App->GetInput()->GetMouseMotionY();

            if (mouseX != 0)
            {
                Quat yawRotation = Quat::RotateY(-mouseX * mouse_rotation_speed);
                frustum.front = yawRotation.Mul(frustum.front).Normalized();
                frustum.up = yawRotation.Mul(frustum.up).Normalized();
            }

            if (mouseY != 0)
            {
                if (frustum.front.y < 0.9f || frustum.front.y > -0.9f) 
                {
                    Quat pitchRotation = Quat::RotateAxisAngle(frustum.WorldRight(), -mouseY * mouse_rotation_speed);
                    frustum.front = pitchRotation.Mul(frustum.front).Normalized();
                    frustum.up = pitchRotation.Mul(frustum.up).Normalized();
                }
            }
        }
       
    }
    // PANNING 
    if (App->GetInput()->IsMouseButtonPressed(SDL_BUTTON_LEFT))
    {
        int mouseX = App->GetInput()->GetMouseMotionX();
        int mouseY = App->GetInput()->GetMouseMotionY();

        if (mouseX != 0)
        {
            frustum.pos -= frustum.WorldRight() * mouseX * pan_speed;
        }

        if (mouseY != 0)
        {
            frustum.pos += frustum.up * mouseY * pan_speed;
        }
    }

    if (App->GetInput()->IsMouseButtonPressed(SDL_BUTTON_LEFT) && App->GetInput()->IsKeyPressed(SDL_SCANCODE_LALT))
    {
        OrbitCamera();
    }

    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_F))
    {
        LOG("HOLA");
        AdaptOnModel(aabbModel);
    }



}

void ModuleCamera::OrbitCamera() {
    
    float3 target = aabbModel.CenterPoint();

    static float yaw = 0.0f;  
    static float pitch = 0.0f; 
    float radius = (frustum.pos - target).Length();


    float deltaX = App->GetInput()->GetMouseMotionX();
    float deltaY = App->GetInput()->GetMouseMotionY();

    float sensitivity = 0.005f; 
    yaw -= deltaX * sensitivity; 
    pitch -= deltaY * sensitivity;

    pitch = Clamp(pitch, -math::pi / 2.0f + 0.1f, math::pi / 2.0f - 0.1f);

    float3 newPosition;
    newPosition.x = target.x + radius * cosf(pitch) * cosf(yaw);
    newPosition.y = target.y + radius * sinf(pitch);
    newPosition.z = target.z + radius * cosf(pitch) * sinf(yaw);

    frustum.pos = newPosition;
    LookAt(target);

}

void ModuleCamera::AdaptOnModel(const AABB& aabb) {

    aabbModel = aabb;

    float3 center = aabb.CenterPoint();

    float3 halfSize = aabb.HalfSize();

    float radius = halfSize.Length() * scalefactor;
    LOG("RADIUS: %d", radius);

    float distance = radius / tanf(frustum.verticalFov * 0.5f);

    float3 newPosition = center - frustum.front * (distance + focus_offset);

    frustum.pos = newPosition;

    LookAt(center);
}

void ModuleCamera::LookAt(const float3& target) {
    frustum.front = (target - frustum.pos).Normalized();

    frustum.up = float3::unitY;
    float3 right = frustum.up.Cross(frustum.front).Normalized();

    frustum.up = frustum.front.Cross(right).Normalized();
}

void ModuleCamera::OnWindowResize(int width, int height) {
    if (height == 0) height = 1; 
    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

}
