#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleDebugDraw.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_opengl3.h"
#include "imgui-docking/imgui_impl_sdl2.h"
#include <windows.h>
#include <psapi.h>

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

        ImGui::Begin("Console");
        for (const auto& line : logBuffer)
        {
            ImGui::TextUnformatted(line.c_str());
        }
        ImGui::End();
        FpsGraph();
        ShowSystemInfoWindow();
        
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

void ModuleEditor::AddLog(const char* log,...) {
    char buf[1024];
    va_list args;
    va_start(args, log);
    vsnprintf(buf, IM_ARRAYSIZE(buf), log, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    logBuffer.push_back(buf);
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
