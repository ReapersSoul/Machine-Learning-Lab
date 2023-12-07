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

class OneHotNode : public NodeInterface {
public:
	OneHotNode() {
		TypeID = "OneHotNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			std::vector<double> output(GetInputDataByIndex(1)[0]["Data"].size(), 0.0);
			for (int i = 0; i < output.size(); i++)
			{
				if (GetInputDataByIndex(0)[0]["Data"].is_array()) {
					if (GetInputDataByIndex(0)[0]["Data"][0] == GetInputDataByIndex(1)[0]["Data"][i])
						output[i] = 1.0;
					else
						output[i] = 0.0;
				}
				else {
					if (GetInputDataByIndex(0)[0]["Data"] == GetInputDataByIndex(1)[0]["Data"][i])
						output[i] = 1.0;
					else
						output[i] = 0.0;
				}
			}
			GetOutputDataByIndex(0)=nlohmann::json::object();
			GetOutputDataByIndex(0)["Data"] = output;
			GetOutputDataByIndex(0)["Type"] = "Vector";
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", nlohmann::json::array());
		MakeInput(1, "Dictionary", "Any", nlohmann::json::array());
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
		return new OneHotNode();
	}
};


extern "C" {
	__declspec(dllexport) std::string GetTypeID() {
		return "OneHotNode";
	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new OneHotNode();
	}
}
