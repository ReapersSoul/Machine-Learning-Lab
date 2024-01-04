#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <UIEngineInterface.hpp>
#include <string>
#include <GLFW/glfw3.h>

class ControllerNode : public NS_Node::NodeInterface {
	bool A;
	bool B;
	bool X;
	bool Y;
	bool LB;
	bool RB;
	bool Select;
	bool Start;
	bool L3;
	bool R3;
	float LX;
	float LY;
	float RX;
	float RY;
	float LT;
	float RT;
	bool DPadUp;
	bool DPadDown;
	bool DPadLeft;
	bool DPadRight;

	int joystick=0;

public:
	ControllerNode() {
		TypeID = "ControllerNode";
	}

	void Init() override {
		unsigned int attribute_one=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Joystick", &joystick);
			}));

		//create Outputs
		unsigned int output_one=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("A "+std::string(A?"True":"False")).c_str());
			});
		unsigned int output_two=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("B "+std::string(B?"True":"False")).c_str());
			});
		unsigned int output_three=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("X "+ std::to_string(X)).c_str());
			});
		unsigned int output_four=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("Y "+std::to_string(Y)).c_str());
			});
		unsigned int output_five=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("LB "+std::string(LB?"True":"False")).c_str());
			});
		unsigned int output_six=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("RB "+std::string(RB?"True":"False")).c_str());
			});
		unsigned int output_seven=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("Select "+std::string(Select?"True":"False")).c_str());
			});
		unsigned int output_eight=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("Start "+std::string(Start?"True":"False")).c_str());
			});
		unsigned int output_nine=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("L3 "+std::string(L3?"True":"False")).c_str());
			});
		unsigned int output_ten=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("R3 "+std::string(R3?"True":"False")).c_str());
			});
		unsigned int output_eleven=MakeOutput(NS_DataObject::GetTypeID("float"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("LX "+std::to_string(LX)).c_str());
			});
		unsigned int output_twelve=MakeOutput(NS_DataObject::GetTypeID("float"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("LY "+std::to_string(LY)).c_str());
			});
		unsigned int output_thirteen=MakeOutput(NS_DataObject::GetTypeID("float"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("RX "+std::to_string(RX)).c_str());
			});
		unsigned int output_fourteen=MakeOutput(NS_DataObject::GetTypeID("float"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("RY "+std::to_string(RY)).c_str());
			});
		unsigned int output_fifteen=MakeOutput(NS_DataObject::GetTypeID("float"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("LT "+std::to_string(LT)).c_str());
			});
		unsigned int output_sixteen=MakeOutput(NS_DataObject::GetTypeID("float"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("RT "+std::to_string(RT)).c_str());
			});
		unsigned int output_seventeen=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("DPadUp "+std::string(DPadUp?"True":"False")).c_str());
			});
		unsigned int output_eighteen=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("DPadDown "+std::string(DPadDown?"True":"False")).c_str());
			});
		unsigned int output_nineteen=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("DPadLeft "+std::string(DPadLeft?"True":"False")).c_str());
			});
		unsigned int output_twenty=MakeOutput(NS_DataObject::GetTypeID("bool"),[this](){
			ImGui::PushItemWidth(100);
			ImGui::Text(("DPadRight "+std::string(DPadRight?"True":"False")).c_str());
			});
		//make lines
		MakeLine(-1,attribute_one,output_one);
		MakeLine(-1,-1,output_two);
		MakeLine(-1,-1,output_three);
		MakeLine(-1,-1,output_four);
		MakeLine(-1,-1,output_five);
		MakeLine(-1,-1,output_six);
		MakeLine(-1,-1,output_seven);
		MakeLine(-1,-1,output_eight);
		MakeLine(-1,-1,output_nine);
		MakeLine(-1,-1,output_ten);
		MakeLine(-1,-1,output_eleven);
		MakeLine(-1,-1,output_twelve);
		MakeLine(-1,-1,output_thirteen);
		MakeLine(-1,-1,output_fourteen);
		MakeLine(-1,-1,output_fifteen);
		MakeLine(-1,-1,output_sixteen);
		MakeLine(-1,-1,output_seventeen);
		MakeLine(-1,-1,output_eighteen);
		MakeLine(-1,-1,output_nineteen);
		MakeLine(-1,-1,output_twenty);

		int l= MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Text("Available Controllers:");
			for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++)
			{
				if (glfwJoystickPresent(i)) {
					ImGui::Text(glfwGetJoystickName(i));
				}
			}
			}));
		MakeLine(-1,l,-1);
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			//get controller values
			// SetOutputByIndexData(0, A);
			// SetOutputByIndexData(1, B);
			// SetOutputByIndexData(2, X);
			// SetOutputByIndexData(3, Y);
			// SetOutputByIndexData(4, LB);
			// SetOutputByIndexData(5, RB);
			// SetOutputByIndexData(6, Select);
			// SetOutputByIndexData(7, Start);
			// SetOutputByIndexData(8, L3);
			// SetOutputByIndexData(9, R3);
			// SetOutputByIndexData(10, LX);
			// SetOutputByIndexData(11, LY);
			// SetOutputByIndexData(12, RX);
			// SetOutputByIndexData(13, RY);
			// SetOutputByIndexData(14, LT);
			// SetOutputByIndexData(15, RT);
			// SetOutputByIndexData(16, DPadUp);
			// SetOutputByIndexData(17, DPadDown);
			// SetOutputByIndexData(18, DPadLeft);
			// SetOutputByIndexData(19, DPadRight);
		}
	}

	void Update(ImGuiContext *Context, GLFWwindow* window) override {
		//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
		//get joystick count
		int present = glfwJoystickPresent(joystick);
		if (present == GLFW_TRUE) {
			//get axis values
			int count;
			const float* axis = glfwGetJoystickAxes(joystick, &count);
			//get button values
			int buttonCount;
			const unsigned char* buttons = glfwGetJoystickButtons(joystick, &buttonCount);

			A = buttons[0];
			B = buttons[1];
			X = buttons[2];
			Y = buttons[3];
			LB = buttons[4];
			RB = buttons[5];
			Select = buttons[6];
			Start = buttons[7];
			L3 = buttons[8];
			R3 = buttons[9];
			LX = axis[0];
			LY = axis[1];
			RX = axis[2];
			RY = axis[3];
			LT = axis[4];
			RT = axis[5];
			DPadUp = buttons[10];
			DPadRight = buttons[11];
			DPadDown = buttons[12];
			DPadLeft = buttons[13];
		}
		else {
			A = false;
			B = false;
			X = false;
			Y = false;
			LB = false;
			RB = false;
			Select = false;
			Start = false;
			L3 = false;
			R3 = false;
			LX = 0;
			LY = 0;
			RX = 0;
			RY = 0;
			LT = 0;
			RT = 0;
			DPadUp = false;
			DPadDown = false;
			DPadLeft = false;
			DPadRight = false;
		}
	}

	void DrawNodeTitle(ImGuiContext *Context, GLFWwindow* window){
				//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
		ImGui::Text("Controller");
	}

	void DrawNodeProperties(ImGuiContext *Context, GLFWwindow* window){
		//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
	}

	NodeInterface* GetInstance() override {
		return new ControllerNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "ControllerNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new ControllerNode();
	}
	
	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}