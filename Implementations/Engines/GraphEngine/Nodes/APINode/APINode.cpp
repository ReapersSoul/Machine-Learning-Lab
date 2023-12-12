#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <APIEngineInterface.hpp>
#include <Graph.hpp>
#include <Attribute.hpp>
#include <string>
#include <GLFW/glfw3.h>

class APINode : public APINodeInterface {
	APIEngineInterface* APIEngine;
	APINode * OutputNode;
	char buffer[256] = "";
	bool EndpointCreated = false;
	nlohmann::json IO;
	enum Type {
		Input,
		Output,
		Expected
	}type = Input;
	bool ShouldProcess = false;
public:
	APINode() {
		TypeID = "APINode";
		type = Input;
	}

	bool ShouldProcessForward() override {
		return ShouldProcess;
	}

	void SetShouldProcess(bool ShouldProcess) override {
		this->ShouldProcess = ShouldProcess;
	}

	void SetOutputNode(APINode* node) {
		OutputNode = node;
	}

	APINode(std::string name, Type i) {
		switch (i) {
		case Input:
			TypeID = "APINode";
			break;
		case Output:
			TypeID = "APINode";
			EndpointCreated = true;
			strcpy_s(buffer, name.c_str());
			break;
		case Expected:
			TypeID = "APINode";
			EndpointCreated = true;
			strcpy_s(buffer, name.c_str());
			break;
		}
		type = i;
	}

	void Init() override {
		if (!DCEE->HasEngineInstance("APIEngine")) {
			#if defined(_MSC_VER)
				APIEngine = DCEE->AddEngineInstance<APIEngineInterface>(DCEE->GetEngine("APIEngine.dll")->GetInstance<APIEngineInterface>());
			#elif defined(__GNUC__)
				APIEngine = DCEE->AddEngineInstance<APIEngineInterface>(DCEE->GetEngine("libAPIEngine.so")->GetInstance<APIEngineInterface>());
			#endif
			APIEngine->Init();
		}
		else {
			APIEngine = DCEE->GetEngineInstanceAs<APIEngineInterface>("APIEngine");
		}
		switch (type) {
		case Input:
			MakeAttribute(0,new Attribute([&]() {
				ImGui::PushItemWidth(100);
				ImGui::InputText("Name", buffer, 256);
				if (std::string(buffer) != "") {
					ImGui::SameLine();
					if (ImGui::Button("Create")) {
						//create output node
						APINode* OutputNode = new APINode(buffer, Output);
						APINode* ExpectedNode = new APINode(buffer, Expected);
						ExpectedNode->SetOutputNode(OutputNode);
						ParentGraph->AddNode(OutputNode);
						ParentGraph->AddNode(ExpectedNode);
						APIEngine->AddEndPoint(buffer, this, OutputNode, ExpectedNode);
						EndpointCreated = true;
						RemoveAttribute(0);
					}
				}
				}));
			MakeOutput(0, "Output", "Any", nlohmann::json::array());
			break;
		case Output:
			MakeInput(0, "Input", "Any", nlohmann::json::array());
			break;
		case Expected:
			MakeOutput(0, "Expected", "Any", nlohmann::json::array());
			break;
		}
	}

	void SetIO(nlohmann::json IO) {
		//copy IO to this->IO
		this->IO = IO;
	}

	std::string GetIO() {
		return IO.dump();
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward&&ShouldProcess) {
			switch (type) {
			case Input:
				GetOutputDataByIndex(0) = IO["Inputs"];
				IO.clear();
				break;
			case Output:
				IO.clear();
				IO["Outputs"] = GetInputDataByIndex(0);
				break;
			case Expected:
				GetOutputDataByIndex(0) = IO["Expected"];
				IO.clear();
				break;
			}
		}
	}

	void DrawNodeTitle(ImGuiContext* Context) {
		ImGui::SetCurrentContext(Context);
		switch (type)
		{
		case APINode::Input:
			ImGui::Text((TypeID + " Input: " + buffer).c_str());
			break;
		case APINode::Output:
			ImGui::Text((TypeID + " Output: " + buffer).c_str());
			break;
		case APINode::Expected:
			ImGui::Text((TypeID + " Expected: " + buffer).c_str());
			break;
		default:
			break;
		}
	}

	void Update() override {
		
	}

	//void DrawNodeTitle();

	NodeInterface* GetInstance() override{
		APINode* node = new APINode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data= NS_Node::NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
		
		return;
	}
};



extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT void CleanUp() {

	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new APINode();
	}
}