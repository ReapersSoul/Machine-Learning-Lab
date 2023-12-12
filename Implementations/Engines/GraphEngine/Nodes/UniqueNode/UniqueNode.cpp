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

class UniqueNode : public NS_Node::NodeInterface {
public:
	UniqueNode() {
		TypeID = "UniqueNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			NS_DataObject::DataObjectInterface* input=GetInputByLine(0)->GetData(0);
		}
		else {
			NS_DataObject::DataObjectInterface* output=GetOutputByLine(0)->GetData(0);
		}
	}

	void Init() override {
		unsigned int input = MakeInput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Input");
			});
		unsigned int output = MakeOutput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Output");
			});

		MakeLine(input, -1, output);
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
		return new UniqueNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "UniqueNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new UniqueNode();
	}
}
