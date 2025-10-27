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

std::vector<DPendulum> DVector = {};
std::vector<SPendulum> SVector = {};
int maxTrail = 10000;

float g = 9.807;
float damping = 0.05f;

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW!\n";
        return -1;
    }
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Double Pendulum Simulation", nullptr, nullptr);
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

    // ----------- ImGui Setup -----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Renderer::SetupImGuiStyle();
    // ----------- Simulation state -----------

    float lastTime = glfwGetTime();
    const float physicsStep = 0.0002;

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // -------- Physics update ----------
        float t = deltaTime;
        while (t > 0.0)
        {
            float dtStep = std::min(physicsStep, t);
            for (DPendulum &s : DVector)
            {
                s = DPendulums::DPendulumUpdate(s, damping, g, dtStep);
                float x2 = s.px + s.L1 * sin(s.theta1) + s.L2 * sin(s.theta2);
                float y2 = s.py - s.L1 * cos(s.theta1) - s.L2 * cos(s.theta2);
                PendulumUtils::PendulumAddTrailPoint(s, x2, y2, maxTrail);
            }
            for (SPendulum& s : SVector) {
				s = SPendulums::SPendulumUpdate(s, damping, g, dtStep);
				float x = s.px + s.L * sin(s.theta);
				float y = s.py - s.L * cos(s.theta);
				PendulumUtils::PendulumAddTrailPoint(s, x, y, maxTrail);
            }
            t -= dtStep;
        }

        // -------- Render OpenGL ----------
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (DPendulum &state : DVector)
        {
            float x1 = state.px + state.L1 * sin(state.theta1);
            float y1 = state.py - state.L1 * cos(state.theta1);
            float x2 = x1 + state.L2 * sin(state.theta2);
            float y2 = y1 - state.L2 * cos(state.theta2);

            glColor3f(1.0f, 1.0f, 1.0f);
            Renderer::drawLine(state.px, state.py, x1, y1);
            Renderer::drawLine(x1, y1, x2, y2);

            glColor3f(1.0f, 0.3f, 0.3f);
            Renderer::drawCircle(x1, y1, 0.03f);
            Renderer::drawCircle(x2, y2, 0.03f);
            glColor3f(0.2f, 0.7f, 0.2f);
            for (std::pair<float, float> &p : state.trailPoints)
                Renderer::drawCircle(p.first, p.second, 0.01f);
        
        }
        for (SPendulum& state : SVector) {
			float x = state.px + state.L * sin(state.theta);
			float y = state.py - state.L * cos(state.theta);
			glColor3f(1.0f, 1.0f, 1.0f);
			Renderer::drawLine(state.px, state.py, x, y);
			glColor3f(0.3f, 0.3f, 1.0f);
			Renderer::drawCircle(x, y, 0.03f);
			glColor3f(0.2f, 0.7f, 0.2f);
			for (std::pair<float, float>& p : state.trailPoints)
				Renderer::drawCircle(p.first, p.second, 0.01f);
        }



        // -------- ImGui Frame ----------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Main Controls");
        if (ImGui::Button("Spawn Double Pendulum"))
            DVector.push_back(DPendulums::DPendulumCreate());
        if(ImGui::Button("Spawn Single Pendulum"))
			SVector.push_back(SPendulums::SPendulumCreate());
        if (ImGui::Button("Delete All Pendulums"))
        {
            DVector.clear();
			SVector.clear();
        }
        ImGui::SliderInt("Max Trail", &maxTrail, 300, 50000);

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
        for (size_t i = 0; i < DVector.size(); i++)
        {
            ImGui::Begin(("Double Pendulum " + std::to_string(i + 1)).c_str());
            ImGui::Checkbox(("Freeze Pendulum " + std::to_string(i + 1)).c_str(), &DVector[i].isFreezed);
            ImGui::Text("Pendulum %d Controls", (int)(i + 1));
            ImGui::Text("Pivoting");
            ImGui::SliderFloat("X Pivot", &DVector[i].px, -2.0f, 2.0f);
            ImGui::SliderFloat("Y Pivot", &DVector[i].py, -1.5f, 1.5f);
            ImGui::Text("Lengths");
            ImGui::SliderFloat("Length 1", &DVector[i].L1, 0.1, 2.0);
            ImGui::SliderFloat("Length 2", &DVector[i].L2, 0.1, 2.0);
            ImGui::Text("Masses");
            ImGui::SliderFloat("Mass 1", &DVector[i].m1, 0.1, 1000.0);
            ImGui::SliderFloat("Mass 2", &DVector[i].m2, 0.1, 1000.0);
            ImGui::Text("Theta Angles (radians)");
            ImGui::SliderFloat("Theta 1", &DVector[i].theta1, -M_PI, M_PI);
            ImGui::SliderFloat("Theta 2", &DVector[i].theta2, -M_PI, M_PI);
            ImGui::Text("Angular Velocities (rad/s)");
            ImGui::SliderFloat("Omega 1", &DVector[i].omega1, -10.0f, 10.0f);
            ImGui::SliderFloat("Omega 2", &DVector[i].omega2, -10.0f, 10.0f);
            if (ImGui::Button(("Delete Pendulum " + std::to_string(i + 1)).c_str()))
            {
                PendulumUtils::PendulumClearTrail(DVector[i]);
                DVector.erase(DVector.begin() + i);
            }
            if (ImGui::Button(("Reset Pendulum " + std::to_string(i + 1)).c_str()))
            {
                DPendulums::DPendulumReset(DVector[i]);
                PendulumUtils::PendulumClearTrail(DVector[i]);
            }
            ImGui::End();
        }

        for (size_t i = 0; i < SVector.size(); i++) {
			ImGui::Begin(("Single Pendulum " + std::to_string(i + 1)).c_str());
			ImGui::Checkbox(("Freeze Pendulum " + std::to_string(i + 1)).c_str(), &SVector[i].isFreezed);
			ImGui::Text("Pendulum %d Controls", (int)(i + 1));
			ImGui::Text("Pivoting");
			ImGui::SliderFloat("X Pivot", &SVector[i].px, -2.0f, 2.0f);
			ImGui::SliderFloat("Y Pivot", &SVector[i].py, -1.5f, 1.5f);
			ImGui::Text("Length");
			ImGui::SliderFloat("Length", &SVector[i].L, 0.1, 2.0);
			ImGui::Text("Mass");
			ImGui::SliderFloat("Mass", &SVector[i].m, 0.1, 1000.0);
			ImGui::Text("Theta Angle (radians)");
			ImGui::SliderFloat("Theta", &SVector[i].theta, -M_PI, M_PI);
			ImGui::Text("Angular Velocity (rad/s)");
			ImGui::SliderFloat("Omega", &SVector[i].omega, -10.0f, 10.0f);
            if (ImGui::Button(("Delete Pendulum " + std::to_string(i + 1)).c_str()))
            {
                PendulumUtils::PendulumClearTrail(SVector[i]);
                SVector.erase(SVector.begin() + i);
            }
            if (ImGui::Button(("Reset Pendulum " + std::to_string(i + 1)).c_str()))
            {
                SPendulums::SPendulumReset(SVector[i]);
                PendulumUtils::PendulumClearTrail(SVector[i]);
			}
			ImGui::End();
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