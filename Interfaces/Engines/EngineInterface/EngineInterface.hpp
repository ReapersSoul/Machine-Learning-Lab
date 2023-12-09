#pragma once
#include <string>

class DynamicCodeExecutionEngineInterface;

class EngineInterface {
protected:
    DynamicCodeExecutionEngineInterface* DCEEngine;
    std::string Name;
public:
    virtual void SetDCEEngine(DynamicCodeExecutionEngineInterface* DCEEngine) {
        this->DCEEngine = DCEEngine;
    }

    virtual std::string GetName() {
		return Name;
	}

    virtual void SetName(std::string name) {
		Name = name;
	}
};