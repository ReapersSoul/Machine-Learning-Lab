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

class FlattenNode : public NodeInterface {
public:
	FlattenNode() {
		TypeID = "FlattenNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data= nlohmann::json::object();
			data["Data"] = GetInputDataByIndex(0)[0]["Data"];
			data["Type"]="Vector";
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
		return new FlattenNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "FlattenNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NodeInterface* GetInstance() {
		return new FlattenNode();
	}
}
