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


/**
 * Constructor for the ModuleCamera class. Initializes the frustum and sets default values.
 */
ModuleCamera::ModuleCamera() {
    frustum.type = FrustumType::PerspectiveFrustum;

    frustum.pos = float3(0.0f, 0.0f, 15.0f);
    frustum.front = -float3::unitZ;
    frustum.up = float3::unitY;

    speed_base = movement_speed;

}

/**
 * Destructor for the ModuleCamera class. Cleans up any allocated resources.
 */
ModuleCamera::~ModuleCamera() {
    if (w != nullptr) {
        delete w;
    }

    if (h != nullptr) {
        delete h;
    }

    App->GetDebugDraw()->CleanUp();
}

/**
 * Initializes the camera module.
 *
 * @return true if initialization was successful.
 */
bool ModuleCamera::Init()
{
    bool ret = true;
    OnWindowResize(App->GetWindow()->width, App->GetWindow()->height);
    return ret;
}

/**
 * Updates the camera module each frame.
 *
 * @return The current update status.
 */
update_status ModuleCamera::Update()
{
    update_status ret = UPDATE_CONTINUE;

    RenderCamera();
    MoveCamera();

    return ret;
}


/**
 * Renders the camera by calculating the projection and view matrices.
 */
void ModuleCamera::RenderCamera() {

    transformX = 2.0f;
    transformY = 0.0f;
    transformZ = 0.0f;

    model = float4x4::FromTRS(float3(transformX, transformY, transformZ),
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

/**
 * Sends the camera's matrices to the GPU as uniforms.
 */
void ModuleCamera::UniformCamera() {
    glUniformMatrix4fv(0, 1, GL_TRUE, &projection[0][0]);
    glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
    glUniformMatrix4fv(2, 1, GL_TRUE, &model[0][0]);
}

/**
 * Handles camera movement based on user input.
 */
void ModuleCamera::MoveCamera() {

    //INCREASES SPEED
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_LSHIFT))
    {
        movement_speed = speed_base * 3;
    }
    else {
        movement_speed = speed_base;
    }

    if (App->GetInput()->IsMouseButtonPressed(SDL_BUTTON_RIGHT))
    {
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
            // ROTATION WITH MOUSE
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

    //ZOOM WITH MOUSE SCROLL
    int mouse_wheel = App->GetInput()->GetMouseWheel();
    if (mouse_wheel != 0) {
        if (mouse_wheel > 0) {
            frustum.pos += frustum.front * zoom_speed;
        }
        else if (mouse_wheel < 0) {
            frustum.pos -= frustum.front * zoom_speed;
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

    //ORBIT
    if (App->GetInput()->IsMouseButtonPressed(SDL_BUTTON_LEFT) && App->GetInput()->IsKeyPressed(SDL_SCANCODE_LALT))
    {
        OrbitCamera();
    }

    //FOCUS ON MODEL
    if (App->GetInput()->IsKeyPressed(SDL_SCANCODE_F))
    {
        AdaptOnModel(aabbModel);
    }



}

/**
 * Orbits the camera around the model.
 */
void ModuleCamera::OrbitCamera() {
    
    float3 target;

    target.x = aabbModel.CenterPoint().x + transformX;
    target.y = aabbModel.CenterPoint().y + transformY;
    target.z = aabbModel.CenterPoint().z + transformZ;

    static float yaw = 0.0f;  
    static float pitch = 0.0f; 
    float radius = (frustum.pos - target).Length();


    float deltaX = App->GetInput()->GetMouseMotionX();
    float deltaY = App->GetInput()->GetMouseMotionY();

    float sensitivity = 0.005f; 
    yaw -= deltaX * sensitivity; 
    pitch -= deltaY * sensitivity;

    pitch = Clamp(pitch, -math::pi/2.0f + 0.1f, math::pi/2.0f - 0.1f);

    float3 newPosition;
    newPosition.x = target.x + radius * cosf(pitch) * cosf(yaw);
    newPosition.y = target.y + radius * sinf(pitch);
    newPosition.z = target.z + radius * cosf(pitch) * sinf(yaw);

    frustum.pos = newPosition;
    LookAt(target);

}

/**
 * Adapts the camera's position and focus to the model using his AABB.
 *
 * @param aabb The Axis-Aligned Bounding Box to adapt to.
 */
void ModuleCamera::AdaptOnModel(const AABB& aabb) {

    aabbModel = aabb;

    float3 center;

    center.x = aabbModel.CenterPoint().x + transformX;
    center.y = aabbModel.CenterPoint().y + transformY;
    center.z = aabbModel.CenterPoint().z + transformZ;

    float3 halfSize = aabb.HalfSize();

    float radius = halfSize.Length() * scalefactor;

    float distance = radius / tanf(frustum.verticalFov * 0.5f);

    float3 newPosition = center - frustum.front * (distance + focus_offset);

    frustum.pos = newPosition;

    LookAt(center);
}

/**
 * Makes the camera look at a target point.
 *
 * @param target The target point to look at.
 */
void ModuleCamera::LookAt(const float3& target) {
    frustum.front = (target - frustum.pos).Normalized();

    frustum.up = float3::unitY;
    float3 right = frustum.up.Cross(frustum.front).Normalized();

    frustum.up = frustum.front.Cross(right).Normalized();
}

/**
 * Updates the camera's aspect ratio when the window is resized.
 *
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
void ModuleCamera::OnWindowResize(int width, int height) {
    if (height == 0) height = 1; 
    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

}
