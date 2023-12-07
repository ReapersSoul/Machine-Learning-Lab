#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <string>
#include <glm/glm.hpp>

#include <typeinfo>

class ViewNode : public NodeInterface {
	std::string display;
public:
	ViewNode() {
		TypeID = "ViewNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			if (!GetInputDataByIndex(0).is_null()) {
				if (!GetInputDataByIndex(0).empty()) {
					display = GetInputDataByIndex(0).dump(4).c_str();
				}
			}
		}
		else {
			if (!GetOutputDataByIndex(0).is_null()) {
				if (!GetOutputDataByIndex(0).empty()) {
					display = GetOutputDataByIndex(0).dump(4).c_str();
				}
			}
		}
	}

	void Init() override {
		MakeInput(0, "Input", "Any", nlohmann::json::array());
		MakeOutput(0, "Input", "Any", nlohmann::json::array());
		MakeAttribute(0, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			//scrollable region
			if (ImGui::BeginChild("scrolling", ImVec2(300, 300), false, ImGuiWindowFlags_HorizontalScrollbar||ImGuiWindowFlags_AlwaysVerticalScrollbar)){
				//display all text
				ImGui::Text(display.c_str());
			}
			ImGui::EndChild();

			}));
	}

	void StandAloneInit() {

	}

	void Update() override {
		
	}

	NodeInterface* GetInstance() override {
		ViewNode* node = new ViewNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
	}
};


extern "C" {
	__declspec(dllexport) std::string GetTypeID() {
		return "ViewNode";
	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new ViewNode();
	}
}
