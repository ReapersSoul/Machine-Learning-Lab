#pragma once
#include "../../../SerializableInterface/SerializableInterface.hpp"
#include "../DataObjectInterface/DataObjectInterface.hpp"
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
	#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input {
	unsigned int UID=-1;
	unsigned int TypeID=0;
	std::vector<DataObjectInterface::DataObjectInterface*> Data;
	std::function<void()> m_DrawFunction;
public:

	Input()=default;
	Input(unsigned int UID,unsigned int TypeID) {
		this->UID = UID;
		this->TypeID = TypeID;
	}

	void SetDrawFunction(std::function<void()> DrawFunction) {
		m_DrawFunction = DrawFunction;
	}

	void Draw(ImGuiContext* Context, GLFWwindow* Window) {
		glfwMakeContextCurrent(Window);
		ImGui::SetCurrentContext(Context);
		m_DrawFunction();
	}

	unsigned int GetTypeID() {
		return TypeID;
	}

	void SetTypeID(unsigned int TypeID) {
		this->TypeID = TypeID;
	}

	void SetUID(unsigned int UID) {
		this->UID = UID;
	}

	unsigned int GetUID() {
		return UID;
	}

	void AddData(DataObjectInterface::DataObjectInterface* Data) {
		if(Data==nullptr) throw std::runtime_error("Data is nullptr");
		if(Data->GetTypeID()!=TypeID) throw std::runtime_error("Data type mismatch");
		this->Data.push_back(Data);
	}

	void RemoveData(DataObjectInterface::DataObjectInterface* Data) {
		if(Data==nullptr) throw std::runtime_error("Data is nullptr");
		std::vector<DataObjectInterface::DataObjectInterface*>::iterator it = std::find(this->Data.begin(), this->Data.end(), Data);
		if(it==this->Data.end()) throw std::runtime_error("Data not found");
		this->Data.erase(it);
	}

	std::vector<DataObjectInterface::DataObjectInterface*>& GetData() {
		return Data;
	}

	void ClearData() {
		Data.clear();
	}
};