#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>

#include <typeinfo>

class ViewNode : public NS_Node::NodeInterface {
	std::string display;
public:
	ViewNode() {
		TypeID = "ViewNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			std::vector<NS_DataObject::DataObjectInterface*> input = GetInputByLine(0)->GetData();
			
		}
		else {
			std::vector<NS_DataObject::DataObjectInterface*> output = GetOutputByLine(0)->GetData();
			
		}
	}

	void Init() override {
		unsigned int input = MakeInput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Input");
			});
		unsigned int attribute = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			//scrollable region
			if (ImGui::BeginChild("scrolling", ImVec2(300, 300), false, ImGuiWindowFlags_HorizontalScrollbar||ImGuiWindowFlags_AlwaysVerticalScrollbar)){
				//display all text
				ImGui::Text(display.c_str());
			}
			ImGui::EndChild();

			}));
		unsigned int output = MakeOutput(NS_DataObject::GetTypeID("Any"), []() {
			ImGui::Text("Output");
			});

		MakeLine(input, attribute, output);
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
		nlohmann::json data = NS_Node::NodeInterface::Serialize();
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "ViewNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new ViewNode();
	}
}
