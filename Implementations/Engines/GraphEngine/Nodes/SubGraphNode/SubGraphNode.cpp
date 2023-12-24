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

class SubGraphNode : public NS_Node::NodeInterface {
	Graph * Graph;
	std::string FilePath;

public:
	SubGraphNode() {
		TypeID = "SubGraphNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {

		}
		else {

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
		return new SubGraphNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "SubGraphNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new SubGraphNode();
	}
	
	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}
