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

class SubGraphOutputNode : public NodeInterface {
	GraphInterface* Graph;
	std::string FilePath;

public:
	SubGraphOutputNode() {
		TypeID = "SubGraphOutputNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data = nlohmann::json::object();
			data["Data"] = GetInputDataByIndex(0)[0]["Data"];
			data["Type"] = "Vector";
			GetOutputDataByIndex(0) = data;
		}
		else {
			nlohmann::json data = nlohmann::json::object();
			data["Data"] = GetOutputDataByIndex(0)[0]["Data"];
			GetInputDataByIndex(0) = data;
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", {});
		MakeOutput(0, "Output", "Any", {});
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
		return new SubGraphOutputNode();
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new SubGraphOutputNode();
	}
}
