#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <UIEngineInterface.h>
#include <string>
#include <GLFW/glfw3.h>

class ControllerNode : public NodeInterface {
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

	int joystick;

public:
	ControllerNode() {
		TypeID = "ControllerNode";
	}

	void Init() override {
		Attributes.push_back(new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Joystick", &joystick);
			}));

		//create Outputs
		MakeOutput(0, "A", "bool", false);
		MakeOutput(1, "B", "bool", false);
		MakeOutput(2, "X", "bool", false);
		MakeOutput(3, "Y", "bool", false);
		MakeOutput(4, "LB", "bool", false);
		MakeOutput(5, "RB", "bool", false);
		MakeOutput(6, "Select", "bool", false);
		MakeOutput(7, "Start", "bool", false);
		MakeOutput(8, "L3", "bool", false);
		MakeOutput(9, "R3", "bool", false);
		MakeOutput(10, "LX", "float", 0);
		MakeOutput(11, "LY", "float", 0);
		MakeOutput(12, "RX", "float", 0);
		MakeOutput(13, "RY", "float", 0);
		MakeOutput(14, "LT", "float", 0);
		MakeOutput(15, "RT", "float", 0);
		MakeOutput(16, "DPadUp", "bool", false);
		MakeOutput(17, "DPadDown", "bool", false);
		MakeOutput(18, "DPadLeft", "bool", false);
		MakeOutput(19, "DPadRight", "bool", false);
	}

	void StandAloneInit() {

	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			//get controller values
			SetOutputByIndexData(0, A);
			SetOutputByIndexData(1, B);
			SetOutputByIndexData(2, X);
			SetOutputByIndexData(3, Y);
			SetOutputByIndexData(4, LB);
			SetOutputByIndexData(5, RB);
			SetOutputByIndexData(6, Select);
			SetOutputByIndexData(7, Start);
			SetOutputByIndexData(8, L3);
			SetOutputByIndexData(9, R3);
			SetOutputByIndexData(10, LX);
			SetOutputByIndexData(11, LY);
			SetOutputByIndexData(12, RX);
			SetOutputByIndexData(13, RY);
			SetOutputByIndexData(14, LT);
			SetOutputByIndexData(15, RT);
			SetOutputByIndexData(16, DPadUp);
			SetOutputByIndexData(17, DPadDown);
			SetOutputByIndexData(18, DPadLeft);
			SetOutputByIndexData(19, DPadRight);
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
		nlohmann::json data = NodeInterface::Serialize();
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
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) void CleanUp() {

	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new ControllerNode();
	}
}