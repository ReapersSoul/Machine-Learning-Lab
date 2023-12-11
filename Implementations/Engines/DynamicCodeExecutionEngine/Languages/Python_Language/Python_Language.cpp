#include <LanguageInterface.hpp>
#include <ScriptInterface.hpp>
#include <NodeInterface.hpp>
#include <Attribute.hpp>

#include <python3.10/Python.h>
#include <iostream>
#include <string>
#include <vector>

class Python_Language : public LanguageInterface {
	PyObject* ctx;
public:

	std::string GetExtension() override {
		return ".py";
	}

	std::string GetName() override {
		return "Python";
	}

	void SetVariable(const char* name, int value) override {


	}

	void SetVariable(const char* name, std::string value) override {
	}

	void SetVariable(const char* name, float value) override {
	}

	void SetVariable(const char* name, double value) override {
	}

	void SetVariable(const char* name, bool value) override {
	}

	void SetVariable(const char* name, void* value, std::string type) override {
	}

	int GetVariableInt(const char* name) override {
		return 0;
	}

	std::string GetVariableString(const char* name) override {
		return "";
	}

	float GetVariableFloat(const char* name)  override {
		return 0.0f;
	}

	double GetVariableDouble(const char* name) override {
		return 0.0;
	}

	bool GetVariableBool(const char* name) override {
		return false;
	}

	void* GetVariablePointer(const char* name, std::string* type) override {
		return nullptr;
	}

	void LoadScript(ScriptInterface* script) override {
	}

	void RunScript(ScriptInterface* script) override {
	}

	void RunString(std::string script) override {
	}

	void RunFile(std::string path) override {
	}

	void CallFunction(const char* name, std::vector<void*> args, std::vector<std::string> types, void* ret) override {
	}

	class ScriptNode : public ScriptInterface, public NodeInterface {};

	//Activation
	double Activation_Activate(double input) override {
		return 0;
	}

	double Activation_ActivateDerivative(double input) override {
		return 0;
	}

	//Loss
	double Loss_CalculateLoss(double input, double target) override {
		return 0;
	}

	double Loss_CalculateLossDerivative(double input, double target) override {
		return 0;
	}

	void CreateNode(void* node) {
		ScriptNode* snode = (ScriptNode*)node;
		//push the node to global lua table
		//if table doesn't exist, create it


	}

	void RegisterFunctions() {
		/*
				lua_pushcfunction(L, Node_MakeInput);
				lua_setglobal(L, "Node_MakeInput");
				lua_pushcfunction(L, Node_MakeOutput);
				lua_setglobal(L, "Node_MakeOutput");
				lua_pushcfunction(L, Node_MakeAttribute);
				lua_setglobal(L, "Node_MakeAttribute");
				lua_pushcfunction(L, Node_GetInputDataByIndex);
				lua_setglobal(L, "Node_GetInputDataByIndex");
				lua_pushcfunction(L, Node_GetOutputDataByIndex);
				lua_setglobal(L, "Node_GetOutputDataByIndex");
				lua_pushcfunction(L, Node_SetInputDataByIndex);
				lua_setglobal(L, "Node_SetInputDataByIndex");
				lua_pushcfunction(L, Node_SetOutputDataByIndex);
				lua_setglobal(L, "Node_SetOutputDataByIndex");
				lua_pushcfunction(L, Node_ImGuiPushItemWidth);
				lua_setglobal(L, "Node_ImGuiPushItemWidth");
				lua_pushcfunction(L, Node_ImGuiInputInt);
				lua_setglobal(L, "Node_ImGuiInputInt");
				lua_pushcfunction(L, Node_ImGuiInputFloat);
				lua_setglobal(L, "Node_ImGuiInputFloat");
		*/




	}

	void Node_Init(void* node) override {
		ScriptNode* snode = (ScriptNode*)node;

		RegisterFunctions();


	}

	void Node_Process(void* node, bool forward) override {
		ScriptNode* snode = (ScriptNode*)node;
		RegisterFunctions();

	}

	Python_Language() {
		//setup duktape context

	}
};

static Python_Language instance = Python_Language();



extern "C" {
	//Define a function that returns the result of adding two numbers
	EXPORT LanguageInterface* GetInstance() {
		return &instance;
	}
}