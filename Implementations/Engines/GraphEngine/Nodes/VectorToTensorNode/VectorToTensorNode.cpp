#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <AttributeInterface.hpp>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>

class DataToTensorNode : public NodeInterface {
public:
	DataToTensorNode() {
		TypeID = "DataToTensorNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			printf("%s\n", GetInputDataByIndex(0)["Data"].dump(4).c_str());
			GetOutputDataByIndex(0) = GetInputDataByIndex(0)["Data"];
		}
		else {
			GetInputDataByIndex(0)["Data"] = GetOutputDataByIndex(0);
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", nlohmann::json::array());
		MakeOutput(0, "Output", "Any", nlohmann::json::array());
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
		return new DataToTensorNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "DataToTensorNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NodeInterface* GetInstance() {
		return new DataToTensorNode();
	}
}
