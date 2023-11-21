#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>

class DataToMatrixNode : public NodeInterface {
	int width = 28;
	int height = 28;

public:
	DataToMatrixNode() {
		TypeID = "VectorToMatrixNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json output = nlohmann::json::object();
			output["Type"] = "Matrix";
			output["Data"] = GetInputDataByIndex(0)[0]["Data"];
			if (GetInputDataByIndex(1)[0]["Data"].size() > 0) {
				width = GetInputDataByIndex(1)[0]["Data"][0];
			}
			output["Width"] = width;
			if (GetInputDataByIndex(2)[0]["Data"].size() > 0) {
				height = GetInputDataByIndex(2)[0]["Data"][0];
			}
			output["Height"] = height;
			GetOutputDataByIndex(0) = output;
		}
		else {
			GetInputDataByIndex(0)=nlohmann::json::object();
			GetInputDataByIndex(0)["Data"] = GetOutputDataByIndex(0)[0]["Data"];
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", nlohmann::json::array());
		MakeInput(1, "width", "Integer", nlohmann::json::array());
		MakeInput(2, "height", "Integer", nlohmann::json::array());
		MakeOutput(0, "Output", "Any", nlohmann::json::array());

		//width and height attributes
		MakeAttribute(0, new AttributeInterface([&]() {
			ImGui::PushItemWidth(100);
				ImGui::InputInt("Width", &width);
			}));

		MakeAttribute(1, new AttributeInterface([&]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Height", &height);
			}));
	}

	void Update() override {

	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		return;
	}

	NodeInterface* GetInstance() {
		return new DataToMatrixNode();
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new DataToMatrixNode();
	}
}
