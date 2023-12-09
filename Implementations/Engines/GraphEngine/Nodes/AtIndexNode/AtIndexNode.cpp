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

class AtIndexNode : public NodeInterface {
	int index = 0;
	int itteration=0;
	int increment_after = 1;
	bool autoincrement = false;
public:
	AtIndexNode() {
		TypeID = "AtIndexNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data = nlohmann::json::object();
			data["Data"]=GetInputDataByIndex(0)[0]["Data"][index% GetInputDataByIndex(0)[0]["Data"].size()];
			GetOutputDataByIndex(0) = data;
			if(autoincrement&&(itteration%increment_after==0))
				index++;

			itteration++;
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", {});
		MakeOutput(0, "Output", "Any", {});

		MakeAttribute(0, new AttributeInterface([&]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Index", &index);
			}));

		MakeAttribute(1, new AttributeInterface([&]() {
			ImGui::Checkbox("Auto Increment", &autoincrement);
			ImGui::SameLine();
			ImGui::InputInt("Incremment After", &increment_after);
			}));
	}

	void Update() override {

	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();
		data["index"] = index;
		data["autoincrement"] = autoincrement;
		data["increment after"] = increment_after;
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
		if(data.contains("index"))
			index = data["index"];
		if (data.contains("autoincrement"))
			autoincrement = data["autoincrement"];
		if (data.contains("increment after"))
			increment_after = data["increment after"];
		return;
	}

	NodeInterface* GetInstance() {
		return new AtIndexNode();
	}
};



extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT NodeInterface* GetInstance() {
		return new AtIndexNode();
	}

	EXPORT std::string GetTypeID() {
		return "AtIndexNode";
	}
}
