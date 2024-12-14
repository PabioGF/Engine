#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_opengl3.h"
#include "imgui-docking/imgui_impl_sdl2.h"
#include <windows.h>
#include <psapi.h>
#include <chrono>

//https://www.youtube.com/watch?v=e1i_a68CgYE

ModuleEditor::ModuleEditor() {
    showWindows = true;
}

ModuleEditor::~ModuleEditor() {

}

bool ModuleEditor::Init()
{
    bool ret = true;
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(App->GetWindow()->window, App->GetOpenGL()->context);
    ImGui_ImplOpenGL3_Init();

    last_time = std::chrono::high_resolution_clock::now();

    return ret;
}

update_status ModuleEditor::PreUpdate()
{
    update_status ret = UPDATE_CONTINUE;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    return ret;
}

update_status ModuleEditor::Update()
{
    update_status ret = UPDATE_CONTINUE;
    //ImGui::ShowDemoWindow();
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("Gui Demo")){
            showcase = !showcase;
        }
        if (ImGui::MenuItem("Documentation")) {
            App->RequestBrowser("https://github.com/ocornut/imgui/tree/docking");
        }
        if (ImGui::MenuItem("About...")) 
        {
            show_about_window = true;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (show_about_window)
    {
        if (ImGui::Begin("About", &show_about_window, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Name:");
            ImGui::SameLine();
            ImGui::TextUnformatted(App->GetOpenGL()->program_name);
            ImGui::Separator();
            ImGui::TextWrapped("Description: Engine made by Pablo for the Advancerd porgramming for AAA videogames Master");
            ImGui::Separator();
            ImGui::Text("Author: Pablo");
            ImGui::Separator();
            ImGui::TextUnformatted("Libraries: ");
            ImGui::SameLine();
            ImGui::TextUnformatted(App->GetOpenGL()->libraries);
            ImGui::TextUnformatted("License: ");
            ImGui::SameLine();
            ImGui::BeginChild("LicenseText", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextWrapped("%s", App->GetOpenGL()->license_text.c_str());
            ImGui::EndChild();
            ImGui::Separator();
            if (ImGui::Checkbox("Show Windows", &showWindows))
            {
                // Acciones adicionales si es necesario
            }
            if (ImGui::Button("Close"))
            {
                show_about_window = false;
            }

            ImGui::End();
        }

    }
    if (showWindows) {
        ImGui::Begin("Console");
        if (logBuffer && !logBuffer->empty()) {
            for (const auto& log : *logBuffer) {
                ImGui::TextUnformatted(log.c_str());
            }
        }
        ImGui::End();
        FpsGraph();
        ShowSystemInfoWindow();
        ConfigurationsMenu();
        
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui_ImplSDL2_ProcessEvent(&App->GetInput()->sdlEvent);

    return ret;
}


bool ModuleEditor::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    return true;
}

void ModuleEditor::FpsGraph() {
    float prev_delta = delta_time;
    delta_time = clock() / CLOCKS_PER_SEC;
    fps_counter++;
    fps = 1000.0 / SDL_GetTicks();
    if (prev_delta != delta_time) {
       LOG("1 SEC: %d", fps_counter);
    }

    //float fps = 1.0f / delta_time;

    fps_log.push_back(fps);
    ms_log.push_back(delta_time); 

    if (fps_log.size() > log_size)
        fps_log.erase(fps_log.begin());
    if (ms_log.size() > log_size)
        ms_log.erase(ms_log.begin());

}

void ModuleEditor::ModelInformation(const std::vector<std::string>& modelInfo, const std::vector<std::string>& textureInfo) {
    if (showWindows) {
        ImGui::Begin("Model Information");

        ImGui::Text("Model Data:");
        ImGui::Separator();

        for (size_t i = 0; i < modelInfo.size(); ++i) {
            ImGui::BulletText("%s", modelInfo[i].c_str());
        }

        ImGui::Spacing();

        ImGui::Text("Texture Data:");
        ImGui::Separator();

        for (size_t i = 0; i < textureInfo.size(); ++i) {
            ImGui::BulletText("%s", textureInfo[i].c_str());
        }

        ImGui::End();
    }

}


void ModuleEditor::ShowSystemInfoWindow() {
    ImGui::Begin("System Information");

    char title[25];
    // FPS Graph
    ImGui::Text("Performance");
    if (!fps_log.empty()) {
        sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
        ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
    }
    if (!ms_log.empty()) {
        sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
        ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
    }

    ImGui::Separator();

    // Memory Consumption (example using dummy data, replace with actual system query)
    ImGui::Text("Memory Consumption");
    size_t total_memory = 0, used_memory = 0;
    GetMemoryInfo(total_memory, used_memory);
    ImGui::Text("Total Memory: %zu MB", total_memory);
    ImGui::Text("Used Memory: %zu MB", used_memory);

    ImGui::Separator();

    // Hardware Detection
    ImGui::Text("Hardware");
    ImGui::Text("CPU Cores: %d", SDL_GetCPUCount());
    ImGui::Text("RAM: %d MB", SDL_GetSystemRAM());
    ImGui::Text("GPU: %s", (const char*)glGetString(GL_RENDERER));
    ImGui::Text("GPU Vendor: %s", (const char*)glGetString(GL_VENDOR));

    ImGui::Separator();

    // Software Versions
    ImGui::Text("Software");
    ImGui::Text("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    ImGui::Text("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
    ImGui::Text("DevIL Version: %s", "1.8.0"); // Replace with actual DevIL version if necessary

    ImGui::Separator();

    ImGui::End();
}

void ModuleEditor::GetMemoryInfo(size_t& total_memory, size_t& used_memory) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    total_memory = memInfo.ullTotalPhys; 
    used_memory = (memInfo.ullTotalPhys - memInfo.ullAvailPhys);
}

void ModuleEditor::ConfigurationsMenu() {
    static GLint wrap_mode_s = GL_REPEAT;  
    static GLint wrap_mode_t = GL_REPEAT;  
    static GLint mag_filter = GL_LINEAR;
    static GLint min_filter = GL_LINEAR;
    static bool mipmaps_enabled = true;
    static int window_width = App->GetWindow()->width; 
    static int window_height = App->GetWindow()->height;
    static int scale_factor = App->GetCamera()->scalefactor;
    static int initial_scale = App->GetCamera()->scalefactor;

    ImGui::Begin("Configuration Settings");

    if (ImGui::CollapsingHeader("Window settings")) {
        if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
            App->GetWindow()->SetFullScreen(fullscreen);
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Resizable", &resizable)) {
            App->GetWindow()->SetResizable(resizable);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Restart to apply");
        }
        if (!resizable) {
            ImGui::BeginDisabled(); 
        }
        if (ImGui::SliderInt("Width", &App->GetWindow()->width, 800, 1920)) {
            SDL_SetWindowSize(App->GetWindow()->window, App->GetWindow()->width, App->GetWindow()->height);
        }
        if (ImGui::SliderInt("Height", &App->GetWindow()->height, 600, 1080)) {
            SDL_SetWindowSize(App->GetWindow()->window, App->GetWindow()->width, App->GetWindow()->height);
        }
        if (!resizable) {
            ImGui::EndDisabled(); 
        }
    }

    if (ImGui::CollapsingHeader("OpenGL settings")) {
        if (ImGui::Checkbox("Depth Test", &depth_test)) {
            App->GetOpenGL()->EnableParameters(depth_test, cull_face);
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Cull Face", &cull_face)) {
            App->GetOpenGL()->EnableParameters(depth_test, cull_face);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Restart to apply");
        }
    }

    if (ImGui::CollapsingHeader("Model settings")) {
        if (ImGui::SliderInt("Scale", &scale_factor, initial_scale, initial_scale+50)) {
            App->GetCamera()->scalefactor = scale_factor;
        }
    }

    if (ImGui::CollapsingHeader("Texture settings")) {
        if (ImGui::Checkbox("Enable Mipmaps", &mipmaps_enabled)) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            if (mipmaps_enabled) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }

        if (ImGui::Combo("Wrap Mode S", (int*)&wrap_mode_s, "GL_REPEAT\0GL_CLAMP_TO_EDGE\0GL_MIRRORED_REPEAT\0")) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode_s);
        }
        if (ImGui::Combo("Wrap Mode T", (int*)&wrap_mode_t, "GL_REPEAT\0GL_CLAMP_TO_EDGE\0GL_MIRRORED_REPEAT\0")) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode_t);
        }

        if (ImGui::Combo("Mag Filter", (int*)&mag_filter, "GL_NEAREST\0GL_LINEAR\0")) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        }
        if (ImGui::Combo("Min Filter", (int*)&min_filter, "GL_NEAREST\0GL_LINEAR\0GL_NEAREST_MIPMAP_NEAREST\0GL_LINEAR_MIPMAP_LINEAR\0")) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        }
    }



    ImGui::End();
}
