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

class DictionaryNode : public NodeInterface {
public:
	DictionaryNode() {
		TypeID = "DictionaryNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			//printf("%s\n", GetInputDataByIndex(0)["Data"].dump(4).c_str());
			GetOutputDataByIndex(0) = GetInputDataByIndex(0)["Data"];
		}
		else {
			GetInputDataByIndex(0)["Data"] = GetOutputDataByIndex(0);
		}
	}

	void Init() override {
		MakeInput(0, "Input", "String", {});
		MakeOutput(0, "Output", "Double", {});
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
		return new DictionaryNode();
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new DictionaryNode();
	}
}
