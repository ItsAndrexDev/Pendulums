#define NOMINMAX

#include <Windows.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Renderer.h"
#include "Pendulums.h"
#include <string>
#define _USE_MATH_DEFINES

std::vector<std::shared_ptr<PendulumLike>> PendulumVec = {};

float g = 9.807;
float damping = 0.05f;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW!\n";
        return -1;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Double Pendulum Simulation", monitor, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    float aspect = static_cast<float>(mode->width) / static_cast<float>(mode->height);

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect, aspect, -1.0, 1.0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // ----------- ImGui Setup -----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Renderer::SetupImGuiStyle();
    // ----------- Simulation state -----------

    float lastTime = glfwGetTime();

    const float physicsStep = 0.001f;

    std::vector<float> trailTimers;
    trailTimers.reserve(128);

    const float trailSample = 0.01f;

    PendulumVec.reserve(128);

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (trailTimers.size() != PendulumVec.size())
        {
            trailTimers.resize(PendulumVec.size(), 0.0f);
        }

        // -------- Physics update ----------
        float t = deltaTime;
        while (t > 0.0f)
        {
            float dtStep = std::min(physicsStep, t);

            for (size_t i = 0; i < PendulumVec.size(); ++i)
            {
                auto& s = PendulumVec[i];
                if (!s) continue;
                s->update(damping, g, dtStep);

                trailTimers[i] += dtStep;
                if (trailTimers[i] >= trailSample)
                {
                    trailTimers[i] = fmodf(trailTimers[i], trailSample);
                    s->AddTrailPoint();
                }
            }
            t -= dtStep;
        }

        // -------- Render OpenGL ----------
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (auto& state : PendulumVec)
        {
            if (state) state->render();
        }

        // -------- ImGui Frame ----------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Main Controls");
        if (ImGui::Button("Spawn Double Pendulum"))
        {
            PendulumVec.push_back(std::make_shared<DPendulum>(/*Thetas*/1.0f, 1.0f, /*Mass*/1.0f, 1.0f, /*Lengths*/0.6f, 0.4f));
            trailTimers.push_back(0.0f);
        }
        if (ImGui::Button("Spawn Single Pendulum"))
        {
            PendulumVec.push_back(std::make_shared<SPendulum>(/*Theta*/1.0f, /*Mass*/1.0f, /*Length*/0.5f));
            trailTimers.push_back(0.0f);
        }
        if (ImGui::Button("Delete All Pendulums"))
        {
            PendulumVec.clear();
            trailTimers.clear();
        }
        
        ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f);

        ImGui::SliderFloat("Gravity", &g, 0.0f, 1000.f);
        if (ImGui::Button("Earth Gravity"))
            g = 9.807f;
        if (ImGui::Button("Moon Gravity"))
            g = 1.62f;
        if (ImGui::Button("Mars Gravity"))
            g = 3.721f;
        if (ImGui::Button("Mercury Gravity"))
            g = 3.7f;
        if (ImGui::Button("Jupiter Gravity"))
            g = 24.79f;
        if (ImGui::Button("Pluto Gravity"))
            g = 0.62f;
        if (ImGui::Button("Sun Gravity"))
            g = 274.0f;

        ImGui::End();

        for (size_t i = 0; i < PendulumVec.size(); ++i)
        {
            PendulumVec[i]->drawUI(i, PendulumVec);
        }
        if (trailTimers.size() != PendulumVec.size())
        {
            trailTimers.resize(PendulumVec.size(), 0.0f);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // -------- Cleanup ----------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
