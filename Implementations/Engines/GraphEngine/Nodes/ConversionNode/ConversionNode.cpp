#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>

#include <typeinfo>

class ConversionNode : public NS_Node::NodeInterface {
public:
	ConversionNode() {
		TypeID = "ConversionNode";

	}

	void Process(bool DirectionForward) override {
		
	}

	void Init() override {

		
	}

	void StandAloneInit() {

	}

	void Update() override {
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

	}

	NodeInterface* GetInstance() {
		return new ConversionNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "ConversionNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new ConversionNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}
