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

class IO {
	unsigned int ParentNodeUID=-1;
	unsigned int UID=-1;
	unsigned int TypeID=0;
	std::vector<NS_DataObject::DataObjectInterface*> Data;
	std::function<void()> m_DrawFunction;
public:
	IO()=default;
	IO(unsigned int UID,unsigned int TypeID) {
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

	void SetParentNodeUID(unsigned int ParentNodeUID) {
		this->ParentNodeUID = ParentNodeUID;
	}

	unsigned int GetParentNodeUID() {
		return ParentNodeUID;
	}

	void AddData(NS_DataObject::DataObjectInterface* Data) {
		if(Data==nullptr) throw std::runtime_error("Data is nullptr");
		if(Data->GetTypeID()!=TypeID) throw std::runtime_error("Data type mismatch");
		this->Data.push_back(Data);
	}

	void RemoveData(NS_DataObject::DataObjectInterface* Data) {
		if(Data==nullptr) throw std::runtime_error("Data is nullptr");
		std::vector<NS_DataObject::DataObjectInterface*>::iterator it = std::find(this->Data.begin(), this->Data.end(), Data);
		if(it==this->Data.end()) throw std::runtime_error("Data not found");
		this->Data.erase(it);
	}

	std::vector<NS_DataObject::DataObjectInterface*>& GetData() {
		return Data;
	}

	NS_DataObject::DataObjectInterface* GetData(unsigned int Index) {
		if(Index>=Data.size()) throw std::runtime_error("Index out of range");
		return Data[Index];
	}

	template<typename T>
	std::vector<T*> GetData() {
		std::vector<T*> Data;
		for (NS_DataObject::DataObjectInterface* DataObject : this->Data) {
			Data.push_back(dynamic_cast<T*>(DataObject));
		}
		return Data;
	}

	template<typename T>
	T* GetData(unsigned int Index) {
		if(Index>=Data.size()) throw std::runtime_error("Index out of range");
		return dynamic_cast<T*>(Data[Index]);
	}

	void ClearData() {
		Data.clear();
	}
};