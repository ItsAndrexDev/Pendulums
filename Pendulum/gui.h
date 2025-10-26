#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "GL/gl.h"
#include <iostream>

namespace gui {
    struct Gl_RGBA {
        GLclampf r, g, b, a;
    };
    void SetupImGuiStyle();

    class Gui {
        int width;
        int height;
    public:
        GLFWwindow* window;
        Gui(int& width, int& height, const char*& title, bool& vsync, void (*setupTheme)());
        ~Gui();
        void Render(Gl_RGBA);
        void NewFrame();
    };
}
