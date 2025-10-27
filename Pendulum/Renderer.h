#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Renderer
{
	void drawLine(float x1, float y1, float x2, float y2);
	void drawCircle(float cx, float cy, float r, int segments = 32);
	void SetupImGuiStyle();
}