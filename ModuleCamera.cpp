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

    aspect_ratio = SCREEN_WIDTH / SCREEN_HEIGHT;

}

ModuleCamera::~ModuleCamera() {
   
}

bool ModuleCamera::Init()
{
    bool ret = true;

    return ret;
}

update_status ModuleCamera::Update()
{
    update_status ret = UPDATE_CONTINUE;

    RenderCamera();
    MoveCamera();

    return ret;
}



void ModuleCamera::RenderCamera() {
    model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
        float4x4::RotateZ(math::pi / 4.0f),
        float3(2.0f, 1.0f, 1.0f));

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
    glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
    glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
    glUniformMatrix4fv(2, 1, GL_TRUE, &projection[0][0]);
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
    if (App->GetInput()->IsMouseButtonPressed(SDL_BUTTON_LEFT)&& App->GetInput()->IsKeyPressed(SDL_SCANCODE_LALT))
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



}

void ModuleCamera::OnWindowResize(int width, int height) {
    if (height == 0) height = 1; // Evitar división por cero
    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

}
