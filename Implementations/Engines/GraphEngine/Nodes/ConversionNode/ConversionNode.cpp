#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <AttributeInterface.hpp>
#include <string>
#include <glm/glm.hpp>

#include <typeinfo>

class ConversionNode : public NodeInterface {
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
		nlohmann::json data = NodeInterface::Serialize();

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
	EXPORT NodeInterface* GetInstance() {
		return new ConversionNode();
	}
}
