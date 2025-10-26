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

#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct PendulumState
{
    double theta1, theta2;
    double omega1, omega2;
    double m1, m2;
    float px, py;
    double L1 = 0.5, L2 = 0.5;
};
struct TracePoint
{
    double x, y;
};

PendulumState pendulum1{ M_PI / 2, M_PI / 2 + 0.3, 0.0, 0.0, 1, 1, -0.8f, 1.0f };
//PendulumState pendulum2{ M_PI / 2, M_PI / 2 + 0.3, 0.0, 0.0, 1, 1, 0.8f, 1.0f };


std::vector<PendulumState> states = {pendulum1};
std::vector<TracePoint> trailPoints;
int maxTrail = 10000;


#pragma region Physics
const double g = 9.81;

// Physics update (Euler integration)
PendulumState update(const PendulumState &s, double dt)
{
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

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW!\n";
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(1440, 1080, "Double Pendulum Simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    // Enable vsync
    glfwSwapInterval(1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -1.5, 1.5, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    

    double lastTime = glfwGetTime();
    double lastFPSUpdate = lastTime;
    int frameCount = 0;

    const double physicsStep = 0.0002; // larger dt for better performance

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Physics integration in small steps
        double t = deltaTime;
        while (t > 0.0)
        {
            double dtStep = std::min(physicsStep, t);
            for (PendulumState &s : states)
            {
                s = update(s, dtStep);
                double x2 = s.px + s.L1 * sin(s.theta1) + s.L2 * sin(s.theta2);
                double y2 = s.py - s.L1 * cos(s.theta1) - s.L2 * cos(s.theta2);
                trailPoints.push_back({x2, y2});
                if (trailPoints.size() > maxTrail)
                    trailPoints.erase(trailPoints.begin());
            }
            t -= dtStep;
        }

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (PendulumState &state : states)
        {
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
        for (TracePoint &p : trailPoints)
            drawCircle(p.x, p.y, 0.01f);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS measurement
        frameCount++;
        if (currentTime - lastFPSUpdate >= 1.0)
        {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastFPSUpdate = currentTime;
        }
    }

    glfwTerminate();
    return 0;
}
