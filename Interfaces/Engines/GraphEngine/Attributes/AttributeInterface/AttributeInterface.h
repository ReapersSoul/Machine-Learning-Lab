#pragma once
#include <functional>
#include <imgui.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class AttributeInterface {
	std::function<void()> m_DrawFunction;
	public:
	AttributeInterface() = default;
	AttributeInterface(std::function<void()> DrawFunction) : m_DrawFunction(DrawFunction) {}

	virtual void SetDrawFunction(std::function<void()> DrawFunction) {
		m_DrawFunction = DrawFunction;
	}

	virtual void Draw(ImGuiContext* Context,GLFWwindow* Window) {
		glfwMakeContextCurrent(Window);
		ImGui::SetCurrentContext(Context);
		m_DrawFunction();
	}
};