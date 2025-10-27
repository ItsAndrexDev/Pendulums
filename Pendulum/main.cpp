#define NOMINMAX
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <string>
#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct PendulumState
{
    float theta1, theta2;
    float omega1, omega2;
    float m1, m2;
    float px, py;
    float L1, L2;
	bool isFreezed = false;
};
struct TracePoint
{
    double x, y;
};

std::vector<PendulumState> states = {};
std::vector<TracePoint> trailPoints;
int maxTrail = 10000;


float g = 9.807;

// Physics update (Euler integration)
PendulumState update(const PendulumState &s, double dt)
{
	if (s.isFreezed) return s;
    double delta = s.theta2 - s.theta1;
    double den1 = (s.m1 + s.m2) * s.L1 - s.m2 * s.L1 * cos(delta) * cos(delta);
    double den2 = (s.L2 / s.L1) * den1;

    double a1 = (s.m2 * s.L1 * s.omega1 * s.omega1 * sin(delta) * cos(delta) +
                 s.m2 * g * sin(s.theta2) * cos(delta) +
                 s.m2 * s.L2 * s.omega2 * s.omega2 * sin(delta) -
                 (s.m1 + s.m2) * g * sin(s.theta1)) /
                den1;

    double a2 = (-s.m2 * s.L2 * s.omega2 * s.omega2 * sin(delta) * cos(delta) +
                 (s.m1 + s.m2) * (g * sin(s.theta1) * cos(delta) -
                                  s.L1 * s.omega1 * s.omega1 * sin(delta) -
                                  g * sin(s.theta2))) /
                den2;

    PendulumState next = s;
    next.omega1 += a1 * dt;
    next.omega2 += a2 * dt;
    next.theta1 += next.omega1 * dt;
    next.theta2 += next.omega2 * dt;
    return next;
}

// Draw helpers
void drawLine(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawCircle(float cx, float cy, float r, int segments = 32)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++)
    {
        float angle = i * 2.0f * M_PI / segments;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }
    glEnd();
}


void SetupImGuiStyle()
{
    // Comfortable Dark Cyan style by SouthCraftX from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 1.0f;
    style.WindowPadding = ImVec2(20.0f, 20.0f);
    style.WindowRounding = 11.5f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(20.0f, 20.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.ChildRounding = 20.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 17.39999961853027f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(20.0f, 3.400000095367432f);
    style.FrameRounding = 11.89999961853027f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.899999618530273f, 13.39999961853027f);
    style.ItemInnerSpacing = ImVec2(7.099999904632568f, 1.799999952316284f);
    style.CellPadding = ImVec2(12.10000038146973f, 9.199999809265137f);
    style.IndentSpacing = 0.0f;
    style.ColumnsMinSpacing = 8.699999809265137f;
    style.ScrollbarSize = 11.60000038146973f;
    style.ScrollbarRounding = 15.89999961853027f;
    style.GrabMinSize = 3.700000047683716f;
    style.GrabRounding = 20.0f;
    style.TabRounding = 9.800000190734863f;
    style.TabBorderSize = 0.0f;
    style.TabCloseButtonMinWidthSelected = 0.0f;
    style.TabCloseButtonMinWidthUnselected = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.09411764889955521f, 0.1019607856869698f, 0.1176470592617989f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1137254908680916f, 0.125490203499794f, 0.1529411822557449f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0313725508749485f, 0.9490196108818054f, 0.843137264251709f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0313725508749485f, 0.9490196108818054f, 0.843137264251709f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6000000238418579f, 0.9647058844566345f, 0.0313725508749485f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1803921610116959f, 0.1882352977991104f, 0.196078434586525f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.1411764770746231f, 0.1647058874368668f, 0.2078431397676468f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.105882354080677f, 0.105882354080677f, 0.105882354080677f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.1294117718935013f, 0.1490196138620377f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0313725508749485f, 0.9490196108818054f, 0.843137264251709f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.125490203499794f, 0.2745098173618317f, 0.572549045085907f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0313725508749485f, 0.9490196108818054f, 0.843137264251709f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2666666805744171f, 0.2901960909366608f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
}


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
    SetupImGuiStyle();
    // ----------- Simulation state -----------


    double lastTime = glfwGetTime();
    double lastFPSUpdate = lastTime;
    int frameCount = 0;
    const double physicsStep = 0.0002;



    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // -------- Physics update ----------
        double t = deltaTime;
        while (t > 0.0) {
            double dtStep = std::min(physicsStep, t);
            for (PendulumState& s : states) {
                s = update(s, dtStep);
                double x2 = s.px + s.L1 * sin(s.theta1) + s.L2 * sin(s.theta2);
                double y2 = s.py - s.L1 * cos(s.theta1) - s.L2 * cos(s.theta2);
                trailPoints.push_back({ x2, y2 });
                if (trailPoints.size() > (size_t)maxTrail) {
                    size_t excess = trailPoints.size() - (size_t)maxTrail;
                    trailPoints.erase(trailPoints.begin(), trailPoints.begin() + excess);
                }
                    
            }
            t -= dtStep;
        }

        // -------- Render OpenGL ----------
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (PendulumState& state : states) {
            double x1 = state.px + state.L1 * sin(state.theta1);
            double y1 = state.py - state.L1 * cos(state.theta1);
            double x2 = x1 + state.L2 * sin(state.theta2);
            double y2 = y1 - state.L2 * cos(state.theta2);

            glColor3f(1.0f, 1.0f, 1.0f);
            drawLine(state.px, state.py, x1, y1);
            drawLine(x1, y1, x2, y2);

            glColor3f(1.0f, 0.3f, 0.3f);
            drawCircle(x1, y1, 0.05f);
            drawCircle(x2, y2, 0.05f);
        }

        glColor3f(0.2f, 0.7f, 0.2f);
        for (TracePoint& p : trailPoints)
            drawCircle(p.x, p.y, 0.01f);

        // -------- ImGui Frame ----------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Main Controls");
		ImGui::Text("Number of Pendulums: %d", (int)states.size());
        if (ImGui::Button("Add Pendulum")) {
            PendulumState pend{ 0, 0, 0.0, 0.0, 1, 1, 0.0f, 0.0f, 0.5f, 0.5f, false};
            states.push_back(pend);
        }
        if(ImGui::Button("Delete All Pendulums")) {
            states.clear();
			trailPoints.clear();
		}
		ImGui::SliderInt("Max Trail", &maxTrail, 300, 50000);


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
            if(ImGui::Button(("Delete Pendulum " + std::to_string(i + 1)).c_str())) 
				states.erase(states.begin() + i);
            if(ImGui::Button(("Reset Pendulum " + std::to_string(i + 1)).c_str())) {
                states[i] = PendulumState{0, 0, 0.0, 0.0, 1, 1, states[i].px, states[i].py, states[i].L1, states[i].L2, states[i].isFreezed};
			}
			ImGui::End();

        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS measurement
        frameCount++;
        if (currentTime - lastFPSUpdate >= 1.0) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastFPSUpdate = currentTime;
        }
    }

    // -------- Cleanup ----------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}





