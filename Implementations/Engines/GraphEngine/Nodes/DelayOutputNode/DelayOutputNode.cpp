#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>

#include <typeinfo>

class DelayOutputNode : public NS_Node::NodeInterface {
	int DelaySteps = 1;
	int Type = 0;
	int LastType = 0;
	std::vector<nlohmann::json> values;
public:
	DelayOutputNode() {
		TypeID = "DelayOutputNode";
	}

	void Process(bool DirectionForward) override {

		
	}

	void Init() override {
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("DelaySteps", &DelaySteps);
			if(DelaySteps < 0) DelaySteps = 0;
			}));
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Combo("Type", &Type, "Double\0Float\0Int\0Bool\0Vec2\0Vec3\0Vec4\0\0");
			}));

	}

	void StandAloneInit() {

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

		data["DelaySteps"] = DelaySteps;
		data["Type"] = Type;
		data["LastType"] = LastType;
		//serialize values
		data["Values"] = nlohmann::json::array();
		for (int i = 0; i < values.size(); i++)
		{
			data["Values"].push_back(values[i]);
		}
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		DelaySteps = data["DelaySteps"];
		Type = data["Type"];
		LastType = data["LastType"];
		//deserialize values
		for (int i = 0; i < data["Values"].size(); i++)
		{
			values.push_back(data["Values"][i]);
		}

		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("DelaySteps", &DelaySteps);
			if (DelaySteps < 0) DelaySteps = 0;
			}));
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Combo("Type", &Type, "Double\0Float\0Int\0Bool\0Vec2\0Vec3\0Vec4\0\0");
			}));

		return;
	}

	NodeInterface* GetInstance() {
		return new DelayOutputNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "DelayOutputNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new DelayOutputNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}
