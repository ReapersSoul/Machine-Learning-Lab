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

class ColumnNode : public NodeInterface {
	std::string ColumnName;
public:
	ColumnNode() {
		TypeID = "ColumnNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			nlohmann::json data = nlohmann::json::object();
			//printf("%s\n", GetInputDataByIndex(0)[0].dump(4).c_str());
			//printf("%s\n", GetInputDataByIndex(0)[0]["Columns"].dump(4).c_str());
			//printf("%s\n", GetInputDataByIndex(0)[0]["Columns"][ColumnName].dump(4).c_str());
			data["Data"] = GetInputDataByIndex(0)[0]["Columns"][ColumnName]["Data"];
			data["Type"] = "Vector";
			GetOutputDataByIndex(0) = data;
		}
		else {
			nlohmann::json data = nlohmann::json::object();
			data["Data"] = GetOutputDataByIndex(0)[0]["Data"];
			GetInputDataByIndex(0) = data;
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", {});
		MakeAttribute(0, new AttributeInterface([&]() {
				ImGui::PushItemWidth(100);
				char buf[100];
				memcpy(buf, ColumnName.c_str(), 100);
				ImGui::InputText("Column Name", buf, 100);
				ColumnName = buf;
			}));
		MakeOutput(0, "Output", "Any", {});
	}

	void Update() override {

	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();
		data["ColumnName"] = ColumnName;
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
		if (data.contains("ColumnName"))
			ColumnName = data["ColumnName"].get<std::string>();
		return;
	}

	NodeInterface* GetInstance() {
		return new ColumnNode();
	}
};



extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT NodeInterface* GetInstance() {
		return new ColumnNode();
	}
	EXPORT std::string GetTypeID() {
		return "ColumnNode";
	}
}
