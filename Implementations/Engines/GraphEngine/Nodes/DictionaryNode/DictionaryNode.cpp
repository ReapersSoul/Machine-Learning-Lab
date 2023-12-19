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

class DictionaryNode : public NS_Node::NodeInterface {
public:
	DictionaryNode() {
		TypeID = "DictionaryNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			//printf("%s\n", GetInputDataByIndex(0)["Data"].dump(4).c_str());
			//GetOutputDataByIndex(0) = GetInputDataByIndex(0)["Data"];
		}
		else {
			//GetInputDataByIndex(0)["Data"] = GetOutputDataByIndex(0);
		}
	}

	void Init() override {
		unsigned int input_one=MakeInput(NS_DataObject::GetTypeID("Text"),[](){
			ImGui::Text("Input");
		});
		unsigned int output_one = MakeOutput(NS_DataObject::GetTypeID("Text"), []() {
			ImGui::Text("Output");
		});
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
		return new DictionaryNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "DictionaryNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new DictionaryNode();
	}
}
