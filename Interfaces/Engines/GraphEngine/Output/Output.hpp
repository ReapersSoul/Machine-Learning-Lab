#pragma once
#include "../../../SerializableInterface/SerializableInterface.hpp"
#include "../DataObjectInterface/DataObjectInterface.hpp"
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

class Output {
	unsigned int UID=-1;
	unsigned int TypeID=0;
	DataObjectInterface::DataObjectInterface* Data=nullptr;
	std::vector<DataObjectInterface::DataObjectInterface*> Data;
	std::function<void()> m_DrawFunction;
public:



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

	void SetData(DataObjectInterface::DataObjectInterface* data) {
		Data = data;
	}

	DataObjectInterface::DataObjectInterface* GetData() {
		return Data;
	}
};