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




std::vector<DPendulum> states = {};
int maxTrail = 10000;

float g = 9.807;
float damping = 0.05f;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW!\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1440, 1080, "Double Pendulum Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -1.5, 1.5, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // ----------- ImGui Setup -----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Renderer::SetupImGuiStyle();
    // ----------- Simulation state -----------


    float lastTime = glfwGetTime();
    const float physicsStep = 0.0002;



    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // -------- Physics update ----------
        float t = deltaTime;
        while (t > 0.0) {
            float dtStep = std::min(physicsStep, t);
            for (DPendulum& s : states) {
                s = DPendulums::DPendulumUpdate(s,damping, g, dtStep);
                float x2 = s.px + s.L1 * sin(s.theta1) + s.L2 * sin(s.theta2);
                float y2 = s.py - s.L1 * cos(s.theta1) - s.L2 * cos(s.theta2);
				DPendulums::DPendulumAddTrailPoint(s, x2, y2, maxTrail);
            }
            t -= dtStep;
        }

        // -------- Render OpenGL ----------
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (DPendulum& state : states) {
            float x1 = state.px + state.L1 * sin(state.theta1);
            float y1 = state.py - state.L1 * cos(state.theta1);
            float x2 = x1 + state.L2 * sin(state.theta2);
            float y2 = y1 - state.L2 * cos(state.theta2);

            glColor3f(1.0f, 1.0f, 1.0f);
            Renderer::drawLine(state.px, state.py, x1, y1);
            Renderer::drawLine(x1, y1, x2, y2);

            glColor3f(1.0f, 0.3f, 0.3f);
            Renderer::drawCircle(x1, y1, 0.05f);
            Renderer::drawCircle(x2, y2, 0.05f);
            glColor3f(0.2f, 0.7f, 0.2f);
            for (std::pair<float,float>& p : state.trailPoints)
				Renderer::drawCircle(p.first, p.second, 0.01f);
        }

        // -------- ImGui Frame ----------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Main Controls");
        if (ImGui::Button("Spawn Double Pendulum"))
            states.push_back(DPendulums::DPendulumCreate());
        

        if(ImGui::Button("Delete All Pendulums")) {
            states.clear();
		}
		ImGui::SliderInt("Max Trail", &maxTrail, 300, 50000);

		ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f);

        ImGui::SliderFloat("Gravity", &g, 0.0f, 1000.f);
		if (ImGui::Button("Earth Gravity")) g = 9.807f;
		if (ImGui::Button("Moon Gravity")) g = 1.62f;
		if (ImGui::Button("Mars Gravity")) g = 3.721f;
		if (ImGui::Button("Mercury Gravity")) g = 3.7f;
		if (ImGui::Button("Jupiter Gravity")) g = 24.79f;
		if (ImGui::Button("Pluto Gravity")) g = 0.62f;
		if (ImGui::Button("Sun Gravity")) g = 274.0f;


        ImGui::End();
        for (size_t i = 0; i < states.size(); i++)
        {
			ImGui::Begin(("Pendulum " + std::to_string(i + 1)).c_str());
			ImGui::Checkbox(("Freeze Pendulum " + std::to_string(i + 1)).c_str(), &states[i].isFreezed);
			ImGui::Text("Pendulum %d Controls", (int)(i + 1));
            ImGui::Text("Pivoting");
            ImGui::SliderFloat("X Pivot", &states[i].px, -2.0f, 2.0f);
            ImGui::SliderFloat("Y Pivot", &states[i].py, -1.5f, 1.5f);
			ImGui::Text("Lengths");
            ImGui::SliderFloat("Length 1", &states[i].L1, 0.1, 2.0);
			ImGui::SliderFloat("Length 2", &states[i].L2, 0.1, 2.0);
			ImGui::Text("Masses");
            ImGui::SliderFloat("Mass 1", &states[i].m1, 0.1, 1000.0);
			ImGui::SliderFloat("Mass 2", &states[i].m2, 0.1, 1000.0);
			ImGui::Text("Theta Angles (radians)");
            ImGui::SliderFloat("Theta 1", &states[i].theta1, -M_PI, M_PI);
			ImGui::SliderFloat("Theta 2", &states[i].theta2, -M_PI, M_PI);
			ImGui::Text("Angular Velocities (rad/s)");
			ImGui::SliderFloat("Omega 1",&states[i].omega1, -10.0f, 10.0f);
            ImGui::SliderFloat("Omega 2",&states[i].omega2, -10.0f, 10.0f);
            if (ImGui::Button(("Delete Pendulum " + std::to_string(i + 1)).c_str())) {
				DPendulums::DpendulumClearTrail(states[i]);
				states.erase(states.begin() + i);
            }
            if(ImGui::Button(("Reset Pendulum " + std::to_string(i + 1)).c_str())) {
				DPendulums::DPendulumReset(states[i]);
				DPendulums::DpendulumClearTrail(states[i]);
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