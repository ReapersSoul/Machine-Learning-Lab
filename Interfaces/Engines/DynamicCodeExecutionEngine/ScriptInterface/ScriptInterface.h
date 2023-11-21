#pragma once
#include "../Languages/LanguageInterface/LanguageInterface.h"
#include <string>
class DynamicCodeExecutionEngineInterface;

class ScriptInterface {
protected:
	DynamicCodeExecutionEngineInterface* DCEEngine;
	LanguageInterface* Language;
	std::string path;
public:
	ScriptInterface() = default;
	ScriptInterface(std::string path) {
		this->path = path;
	}

	virtual void SetDCEEngine(DynamicCodeExecutionEngineInterface* dceEngine) {
		DCEEngine = dceEngine;
	}

	virtual void SetPath(std::string path) {
		this->path = path;
	}
	virtual std::string GetPath() {
		return path;
	}
	virtual void SetLanguage(LanguageInterface* language) {
		Language = language;
	}
	virtual LanguageInterface* GetLanguage() {
		return Language;
	}

	void SetVariable(const char* name, int value);
	void SetVariable(const char* name, std::string value);
	void SetVariable(const char* name, float value);
	void SetVariable(const char* name, double value);
	void SetVariable(const char* name, bool value);
	void SetVariable(const char* name, void* value, std::string type);

	int GetVariable(const char* name);
	std::string GetVariableString(const char* name);
	float GetVariableFloat(const char* name);
	double GetVariableDouble(const char* name);
	bool GetVariableBool(const char* name);
	void* GetVariablePointer(const char* name, std::string* type);



};