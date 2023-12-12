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

class OneHotNode : public NS_Node::NodeInterface {
public:
	OneHotNode() {
		TypeID = "OneHotNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			NS_DataObject::DataObjectInterface* input = GetInputByLine(0)->GetData(0);
			
		}
	}

	void Init() override {
		unsigned int input = MakeInput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Input");
			});
		unsigned int input2 = MakeInput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Dictionary");
			});
		unsigned int output = MakeOutput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Output");
			});

		MakeLine(input,-1, output);
		MakeLine(input2, -1,-1);
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
		return new OneHotNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "OneHotNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new OneHotNode();
	}
}
