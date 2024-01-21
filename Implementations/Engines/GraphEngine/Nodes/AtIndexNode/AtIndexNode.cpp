#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>

class AtIndexNode : public NS_Node::NodeInterface {
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
			//data["Data"]=GetInputDataByIndex(0)[0]["Data"][index% GetInputDataByIndex(0)[0]["Data"].size()];
			//GetOutputDataByIndex(0) = data;
			if(autoincrement&&(itteration%increment_after==0))
				index++;

			itteration++;
		}
	}

	void Init() override {
		unsigned int input_one=MakeInput(NS_DataObject::GetTypeID("Any"), [](){
			ImGui::Text("Input");
		});
		unsigned int output_one=MakeOutput(NS_DataObject::GetTypeID("Any"), [](){
			ImGui::Text("Output");
		});

		unsigned int attribute_one=MakeAttribute(new Attribute([&]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Index", &index);
			}));

		unsigned int attribute_two=MakeAttribute(new Attribute([&]() {
			ImGui::Checkbox("Auto Increment", &autoincrement);
			ImGui::SameLine();
			ImGui::InputInt("Incremment After", &increment_after);
			}));
	}

	void Update(ImGuiContext *Context, GLFWwindow* window) override {
		//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
	}

	void DrawNodeTitle(ImGuiContext *Context, GLFWwindow* window){
				//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);

	}

	void DrawNodeProperties(ImGuiContext *Context, GLFWwindow* window){
		//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();
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
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new AtIndexNode();
	}

	EXPORT std::string GetTypeID() {
		return "AtIndexNode";
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}
