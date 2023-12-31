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
		unsigned int output_one=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("A");
			});
		unsigned int output_two=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("B");
			});
		unsigned int output_three=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("X");
			});
		unsigned int output_four=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("Y");
			});
		unsigned int output_five=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("LB");
			});
		unsigned int output_six=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("RB");
			});
		unsigned int output_seven=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("Select");
			});
		unsigned int output_eight=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("Start");
			});
		unsigned int output_nine=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("L3");
			});
		unsigned int output_ten=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("R3");
			});
		unsigned int output_eleven=MakeOutput(NS_DataObject::GetTypeID("float"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("LX");
			});
		unsigned int output_twelve=MakeOutput(NS_DataObject::GetTypeID("float"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("LY");
			});
		unsigned int output_thirteen=MakeOutput(NS_DataObject::GetTypeID("float"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("RX");
			});
		unsigned int output_fourteen=MakeOutput(NS_DataObject::GetTypeID("float"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("RY");
			});
		unsigned int output_fifteen=MakeOutput(NS_DataObject::GetTypeID("float"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("LT");
			});
		unsigned int output_sixteen=MakeOutput(NS_DataObject::GetTypeID("float"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("RT");
			});
		unsigned int output_seventeen=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("DPadUp");
			});
		unsigned int output_eighteen=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("DPadDown");
			});
		unsigned int output_nineteen=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("DPadLeft");
			});
		unsigned int output_twenty=MakeOutput(NS_DataObject::GetTypeID("bool"),[](){
			ImGui::PushItemWidth(100);
			ImGui::Text("DPadRight");
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

	void Update() override {
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