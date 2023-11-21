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

class UniqueNode : public NodeInterface {
public:
	UniqueNode() {
		TypeID = "UniqueNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data = nlohmann::json::object();
			//find all unique values in the input
			nlohmann::json input = GetInputDataByIndex(0)[0]["Data"];
			nlohmann::json output= std::vector<nlohmann::json>();

			for (auto& element : input) {
				bool found = false;
				for (auto& element2 : output) {
					if (element == element2) {
						found = true;
						break;
					}
				}
				if (!found) {
					output.push_back(element);
				}
			}

			data["Data"] = output;
			data["Type"] = "Vector";
			GetOutputDataByIndex(0) = data;
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
		return new UniqueNode();
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new UniqueNode();
	}
}
