#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>

class FlattenNode : public NS_Node::NodeInterface {
public:
	FlattenNode() {
		TypeID = "FlattenNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data= nlohmann::json::object();
			//data["Data"] = GetInputDataByIndex(0)[0]["Data"];
			data["Type"]="Vector";
			//GetOutputDataByIndex(0) = data;
		}
		else {
			nlohmann::json data = nlohmann::json::object();
			//data["Data"] = GetOutputDataByIndex(0)[0]["Data"];
			//GetInputDataByIndex(0) = data;
		}
	}

	void Init() override {
		unsigned int input_one=MakeInput(NS_DataObject::GetTypeID("Any"), [](){
			ImGui::Text("Input");
		});
		unsigned int output_one= MakeOutput(NS_DataObject::GetTypeID("Any"), [](){
			ImGui::Text("Output");
		});

		MakeLine(input_one, -1, output_one);
	}

	void Update() override {

	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();

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
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new FlattenNode();
	}
}
