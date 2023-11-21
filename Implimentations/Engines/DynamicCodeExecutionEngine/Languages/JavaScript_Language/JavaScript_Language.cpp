#include <LanguageInterface.h>
#include <ScriptInterface.h>
#include <NodeInterface.h>
#include <AttributeInterface.h>

#include <duktape.h>
#include <iostream>
#include <string>
#include <vector>

class JavaScript_Language : public LanguageInterface {
	duk_context* ctx;
public:

	std::string GetExtension() override {
		return ".js";
	}

	std::string GetName() override {
		return "JavaScript";
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

	static int Node_MakeInput(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int line = duk_get_int(ctx, 1);
		std::string name = duk_get_string(ctx, 2);
		std::string type = duk_get_string(ctx, 3);
		node->MakeInput(line,name,type,nlohmann::json::parse(duk_get_string(ctx, 4)));
		return 0;
	}

	static int Node_MakeOutput(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int line = duk_get_int(ctx, 1);
		std::string name = duk_get_string(ctx, 2);
		std::string type = duk_get_string(ctx, 3);
		node->MakeOutput(line, name, type, nlohmann::json::parse(duk_get_string(ctx, 4)));
		return 0;
	}

	static int Node_MakeAttribute(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int line = duk_get_int(ctx, 1);
		std::string name = duk_get_string(ctx, 2);
		std::string type = duk_get_string(ctx, 3);
		//node->MakeAttribute(line, name, type, nlohmann::json::parse(duk_get_string(ctx, 4)));
		return 0;
	}

	static int Node_GetInputDataByIndex(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int index = duk_get_int(ctx, 1);
		duk_push_string(ctx, node->GetInputDataByIndex(index).dump().c_str());
		return 1;
	}

	static int Node_GetOutputDataByIndex(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int index = duk_get_int(ctx, 1);
		duk_push_string(ctx, node->GetOutputDataByIndex(index).dump().c_str());
		return 1;
	}

	static int Node_SetInputDataByIndex(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int index = duk_get_int(ctx, 1);
		node->GetInputDataByIndex(index) = nlohmann::json::parse(duk_get_string(ctx, 2));
		return 0;
	}

	static int Node_SetOutputDataByIndex(duk_context* ctx) {
		ScriptNode* node = (ScriptNode*)duk_get_pointer(ctx, 0);
		int index = duk_get_int(ctx, 1);
		node->GetOutputDataByIndex(index) = nlohmann::json::parse(duk_get_string(ctx, 2));
		return 0;
	}


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
		//ScriptNode* snode = (ScriptNode*)node;
		////push the node to the node array on the stack
		//duk_push_global_object(ctx);
		//duk_push_string(ctx, "nodes");
		//duk_get_prop(ctx, -2);
		//duk_push_number(ctx, snode->GetUID());
		//duk_push_pointer(ctx, node);
		//duk_put_prop(ctx, -3);
		//duk_pop(ctx);
		//duk_pop(ctx);
	}

	void RegisterFunctions() {
		//register Node_MakeInput
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_MakeInput");
		duk_push_c_function(ctx, Node_MakeInput, 5);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
		//register Node_MakeOutput
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_MakeOutput");
		duk_push_c_function(ctx, Node_MakeOutput, 5);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
		//register Node_MakeAttribute
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_MakeAttribute");
		duk_push_c_function(ctx, Node_MakeAttribute, 5);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
		//register Node_GetInputDataByIndex
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_GetInputDataByIndex");
		duk_push_c_function(ctx, Node_GetInputDataByIndex, 2);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
		//register Node_GetOutputDataByIndex
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_GetOutputDataByIndex");
		duk_push_c_function(ctx, Node_GetOutputDataByIndex, 2);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
		//register Node_SetInputDataByIndex
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_SetInputDataByIndex");
		duk_push_c_function(ctx, Node_SetInputDataByIndex, 3);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
		//register Node_SetOutputDataByIndex
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Node_SetOutputDataByIndex");
		duk_push_c_function(ctx, Node_SetOutputDataByIndex, 3);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);

	}

	void Node_Init(void* node) override {
		ScriptNode* snode = (ScriptNode*)node;

		RegisterFunctions();

		//load the script Using Get Path
		std::string path = snode->GetPath();
		std::fstream file(path);
		std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		//push the script to the stack
		duk_push_string(ctx, script.c_str());
		//evaluate the script
		if (duk_peval(ctx) != 0) {
			printf("Error in script: %s\n", duk_safe_to_string(ctx, -1));
		}
		//pop the script
		duk_pop(ctx);

		//push the node to the init function
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Init");
		duk_get_prop(ctx, -2);
		duk_push_pointer(ctx, node);
		//call and error check
		if (duk_pcall(ctx, 1) != 0) {
			printf("Error in script: %s\n", duk_safe_to_string(ctx, -1));
		}
		duk_pop(ctx);
		duk_pop(ctx);

	}

	void Node_Process(void* node, bool forward) override {
		ScriptNode* snode = (ScriptNode*)node;
		RegisterFunctions();

		//load the script Using Get Path
		std::string path = snode->GetPath();
		std::fstream file(path);
		std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		//push the script to the stack
		duk_push_string(ctx, script.c_str());
		//evaluate the script
		if (duk_peval(ctx) != 0) {
			printf("Error in script: %s\n", duk_safe_to_string(ctx, -1));
		}
		//pop the script
		duk_pop(ctx);

		//push the node to the process function
		duk_push_global_object(ctx);
		duk_push_string(ctx, "Process");
		duk_get_prop(ctx, -2);
		duk_push_pointer(ctx, node);
		duk_push_boolean(ctx, forward);
		if (duk_pcall(ctx, 2) != 0) {
			printf("Error in script: %s\n", duk_safe_to_string(ctx, -1));
		}
		duk_pop(ctx);
		duk_pop(ctx);
	}
	//print function
	static int print(duk_context* ctx) {
		printf("%s\n", duk_to_string(ctx, 0));
		return 0;
	}

	JavaScript_Language() {
		//setup duktape context
		ctx = duk_create_heap_default();
		//register the print function
		duk_push_global_object(ctx);
		duk_push_string(ctx, "print");
		duk_push_c_function(ctx, print, DUK_VARARGS);
		duk_put_prop(ctx, -3);
		duk_pop(ctx);
	}
};

static JavaScript_Language instance = JavaScript_Language();

extern "C" {
	//Define a function that returns the result of adding two numbers
	__declspec(dllexport) LanguageInterface* GetInstance() {
		return &instance;
	}
}