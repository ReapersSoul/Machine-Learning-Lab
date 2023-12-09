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

class NormalizeNode : public NodeInterface {
	std::vector<double> input;

	std::vector<double> Normalize(std::vector<double> data) {
		double min = *std::min_element(data.begin(), data.end());
		double max = *std::max_element(data.begin(), data.end());

		for (auto& value : data) {
			value = (value - min) / (max - min);
		}

		return data;
	}

	std::vector<double> DerivativeNormalize(const std::vector<double>& data) {
		double min = *std::min_element(data.begin(), data.end());
		double max = *std::max_element(data.begin(), data.end());
		double range = max - min;

		std::vector<double> derivative(data.size(), 1 / range);

		return derivative;
	}

public:
	NormalizeNode() {
		TypeID = "NormalizeNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data = nlohmann::json::object();
			input.resize(GetInputDataByIndex(0)[0]["Data"].size());
			for (int i = 0; i < input.size(); i++) {
				input[i] = GetInputDataByIndex(0)[0]["Data"][i];
			}
			data["Data"] = Normalize(input);
			data["Type"] = "Vector";
			GetOutputDataByIndex(0) = data;
		}
		else {
			nlohmann::json data = nlohmann::json::object();
			std::vector<double> FG=GetOutputDataByIndex(0)[0]["Data"];
			std::vector<double> derivative = DerivativeNormalize(input);
			for (int i = 0; i < input.size(); i++) {
				FG[i] *= derivative[i];
			}
			data["Data"] = FG;
			data["Type"] = "Vector";
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
		return new NormalizeNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "NormalizeNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NodeInterface* GetInstance() {
		return new NormalizeNode();
	}
}
