#include "../../../../Util/Exports.hpp"
#include <LanguageInterface.hpp>
#include <ScriptInterface.hpp>
#include <NodeInterface.hpp>
#include <Attribute.hpp>

#include <lua.hpp>
#include <iostream>
#include <string>
#include <vector>

class Lua_Language : public LanguageInterface {
	lua_State* L;
public:

	std::string GetExtension() override {
		return ".lua";
	}

	std::string GetName() override {
		return "Lua";
	}

	void SetVariable(const char* name, int value) override {
		lua_pushinteger(L, value);
		lua_setglobal(L, name);

	}

	void SetVariable(const char* name, std::string value) override {
		lua_pushstring(L, value.c_str());
		lua_setglobal(L, name);
	}

	void SetVariable(const char* name, float value) override {
		lua_pushnumber(L, value);
		lua_setglobal(L, name);
	}

	void SetVariable(const char* name, double value) override {
		lua_pushnumber(L, value);
		lua_setglobal(L, name);
	}

	void SetVariable(const char* name, bool value) override {
		lua_pushboolean(L, value);
		lua_setglobal(L, name);
	}

	void SetVariable(const char* name, void* value, std::string type) override {
		lua_pushlightuserdata(L, value);
		lua_setglobal(L, name);
	}

	int GetVariableInt(const char* name) override {
		lua_getglobal(L, name);
		return lua_tointeger(L, -1);
	}

	std::string GetVariableString(const char* name) override {
		lua_getglobal(L, name);
		return lua_tostring(L, -1);
	}

	float GetVariableFloat(const char* name)  override {
		lua_getglobal(L, name);
		return lua_tonumber(L, -1);
	}

	double GetVariableDouble(const char* name) override {
		lua_getglobal(L, name);
		return lua_tonumber(L, -1);
	}

	bool GetVariableBool(const char* name) override {
		lua_getglobal(L, name);
		return 0; lua_toboolean(L, -1);
	}

	void* GetVariablePointer(const char* name, std::string* type) override {
		lua_getglobal(L, name);
		return lua_touserdata(L, -1);
	}

	void LoadScript(ScriptInterface* script) override {
		if (luaL_loadfile(L, script->GetPath().c_str()) || lua_pcall(L, 0, 0, 0)) {
			std::cout << "error: " << lua_tostring(L, -1) << std::endl;
		}
	}

	void RunScript(ScriptInterface* script) override {
		if (luaL_loadfile(L, script->GetPath().c_str()) || lua_pcall(L, 0, 0, 0)) {
			std::cout << "error: " << lua_tostring(L, -1) << std::endl;
		}
	}

	void RunString(std::string script) override {
		if (luaL_loadstring(L, script.c_str()) || lua_pcall(L, 0, 0, 0)) {
			std::cout << "error: " << lua_tostring(L, -1) << std::endl;
		}
	}

	void RunFile(std::string path) override {
		if (luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0)) {
			std::cout << "error: " << lua_tostring(L, -1) << std::endl;
		}
	}

	void CallFunction(const char* name, std::vector<void*> args, std::vector<std::string> types, void* ret) override {
		lua_getglobal(L, name);
		for (int i = 0; i < args.size(); i++)
		{
			if (types[i] == "int") {
				lua_pushinteger(L, *(int*)args[i]);
			}
			else if (types[i] == "string") {
				lua_pushstring(L, (char*)args[i]);
			}
			else if (types[i] == "float") {
				lua_pushnumber(L, *(float*)args[i]);
			}
			else if (types[i] == "double") {
				lua_pushnumber(L, *(double*)args[i]);
			}
			else if (types[i] == "bool") {
				lua_pushboolean(L, *(bool*)args[i]);
			}
			else if (types[i] == "pointer") {
				lua_pushlightuserdata(L, args[i]);
			}
		}
		//handle errors by printing them out
		if (lua_pcall(L, args.size(), 1, 0) != 0) {
			std::cout << lua_tostring(L, -1) << std::endl;
		}
		ret = lua_touserdata(L, -1);
	}

	class ScriptNode : public ScriptInterface, public NodeInterface {};

	void CreateNode(void* node) {
		ScriptNode* snode = (ScriptNode*)node;
		//push the node to global lua table
		//if table doesn't exist, create it
		lua_getglobal(L, "nodes");
		if (lua_isnil(L, -1)) {
			lua_newtable(L);
			lua_setglobal(L, "nodes");
			lua_getglobal(L, "nodes");
		}
		//push the node to the table
		lua_pushlightuserdata(L, snode);
		lua_setfield(L, -2, std::to_string(snode->GetUID()).c_str());
		//pop the table
		lua_pop(L, 1);

	}

	static int Node_MakeInput(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		node->MakeInput(lua_tointeger(L, 2), lua_tostring(L, 3), lua_tostring(L, 4), nlohmann::json::parse(lua_tostring(L, 5)));
		return 0; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_MakeOutput(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		node->MakeOutput(lua_tointeger(L, 2), lua_tostring(L, 3), lua_tostring(L, 4), nlohmann::json::parse(lua_tostring(L, 5)));
		return 0; //number of return values to lua, 0 means no return values to lua
	}



	static int Node_MakeAttribute(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int line = lua_tointeger(L, 2);
		int function=luaL_ref(L, LUA_REGISTRYINDEX);
		node->MakeAttribute(line, new Attribute([&L,node,function]() {
			lua_rawgeti(L, LUA_REGISTRYINDEX, function);
			if (lua_pcall(L, 0, 0, 0) != 0) {
				std::cout << lua_tostring(L, -1) << std::endl;
			}
			}));
		return 0; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_GetInputDataByIndex(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int index = lua_tointeger(L, 2);
		std::string json=node->GetInputDataByIndex(index).dump();
		lua_pushstring(L, json.c_str());
		return 1; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_GetOutputDataByIndex(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int index = lua_tointeger(L, 2);
		std::string json = node->GetOutputDataByIndex(index).dump();
		lua_pushstring(L, json.c_str());
		return 1; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_SetInputDataByIndex(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int index = lua_tointeger(L, 2);
		std::string json = lua_tostring(L, 3);
		node->GetInputDataByIndex(index) = nlohmann::json::parse(json);
		return 0; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_SetOutputDataByIndex(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int index = lua_tointeger(L, 2);
		std::string json = lua_tostring(L, 3);
		node->GetOutputDataByIndex(index) = nlohmann::json::parse(json);
		return 0; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_ImGuiPushItemWidth(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		float width = lua_tonumber(L, 2);
		ImGui::PushItemWidth(width);
		return 0; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_ImGuiInputInt(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int index = lua_tointeger(L, 2);
		std::string label = lua_tostring(L, 3);
		int v = node->GetInputDataByIndex(index).get<int>();
		ImGui::InputInt(label.c_str(), &v);
		node->GetInputDataByIndex(index) = v;
		return 0; //number of return values to lua, 0 means no return values to lua
	}

	static int Node_ImGuiInputFloat(lua_State* L) {
		//get the node
		ScriptNode* node = (ScriptNode*)lua_touserdata(L, 1);
		int index = lua_tointeger(L, 2);
		std::string label = lua_tostring(L, 3);
		float v = node->GetInputDataByIndex(index).get<float>();
		ImGui::InputFloat(label.c_str(), &v);
		node->GetInputDataByIndex(index) = v;
		return 0; //number of return values to lua, 0 means no return values to lua{
	}

	
	//Activation
	double Activation_Activate(double input) override {
		//get the function from lua
		lua_getglobal(L, "Activate");
		//push the input to the stack
		lua_pushnumber(L, input);
		//call the function with 1 argument, 1 return value
		if (lua_pcall(L, 1, 1, 0) != 0) {
			std::cout << lua_tostring(L, -1) << std::endl;
		}
		//get the return value
		double output = lua_tonumber(L, -1);
		//pop the return value from the stack
		lua_pop(L, 1);
		return output;
	}
	
	double Activation_ActivateDerivative(double input) override{
		//get the function from lua
		lua_getglobal(L, "ActivateDerivative");
		//push the input to the stack
		lua_pushnumber(L, input);
		//call the function with 1 argument, 1 return value
		if (lua_pcall(L, 1, 1, 0) != 0) {
			std::cout << lua_tostring(L, -1) << std::endl;
		}
		//get the return value
		double output = lua_tonumber(L, -1);
		//pop the return value from the stack
		lua_pop(L, 1);
		return output;
	}

	//Loss
	double Loss_CalculateLoss(double input, double target) override {
		//get the function from lua
		lua_getglobal(L, "CalculateLoss");
		lua_pushnumber(L, input);
		lua_pushnumber(L, target);
		if (lua_pcall(L, 2, 1, 0) != 0) {
			printf("ERROR: %s\n", lua_tostring(L, -1));
		}
		double output = lua_tonumber(L, -1);
		lua_pop(L,1);
		return output;
	}
	
	double Loss_CalculateLossDerivative(double input, double target) override {
		//get the function from lua
		lua_getglobal(L, "CalculateLossDerivative");
		lua_pushnumber(L, input);
		lua_pushnumber(L, target);
		if (lua_pcall(L, 2, 1, 0) != 0) {
			printf("ERROR: %s\n", lua_tostring(L, -1));
		}
		double output = lua_tonumber(L, -1);
		lua_pop(L, 1);
		return output;
	}



	void RegisterFunctions() {
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

	}

	void Node_Init(void* node) override {
		ScriptNode* snode = (ScriptNode*)node;

		RegisterFunctions();


		//load the script
		luaL_loadfile(L, snode->GetPath().c_str());
		//find global function Init
		lua_getglobal(L, "Init");
		//push the node
		lua_pushlightuserdata(L, snode);

		if (lua_pcall(L, 1, 0, 0) != 0) {
			std::cout << lua_tostring(L, -1) << std::endl;
		}

	}

	void Node_Process(void* node,bool forward) override {
		ScriptNode* snode = (ScriptNode*)node;
		RegisterFunctions();
		//load the script
		luaL_loadfile(L, snode->GetPath().c_str());
		//find global function Process
		lua_getglobal(L, "Process");
		//push the node
		lua_pushlightuserdata(L, snode);
		//push the forward bool
		lua_pushboolean(L, forward);
		if (lua_pcall(L, 2, 0, 0) != 0) {
			std::cout << lua_tostring(L, -1) << std::endl;
		}
	}

	Lua_Language() {
		L = luaL_newstate();
		luaL_openlibs(L);
	}
};

static Lua_Language instance = Lua_Language();



extern "C" {
	//Define a function that returns the result of adding two numbers
	EXPORT LanguageInterface* GetInstance() {
		return &instance;
	}
}