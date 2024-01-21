#pragma once
#include <functional>
//imgui
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Attribute {
	std::function<void()> m_DrawFunction;
	public:
	Attribute() = default;
	Attribute(std::function<void()> DrawFunction) : m_DrawFunction(DrawFunction) {}

	virtual void SetDrawFunction(std::function<void()> DrawFunction) {
		m_DrawFunction = DrawFunction;
	}

	virtual void Draw(ImGuiContext* Context,GLFWwindow* Window) {
		glfwMakeContextCurrent(Window);
		ImGui::SetCurrentContext(Context);
		m_DrawFunction();
	}
};