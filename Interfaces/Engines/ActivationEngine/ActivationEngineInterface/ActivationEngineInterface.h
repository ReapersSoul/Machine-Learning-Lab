#pragma once
#include "../../EngineInterface/EngineInterface.h"
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.h"
#include "../Activations/ActivationInterface/ActivationInterface.h"

class ActivationEngineInterface : public EngineInterface {
protected:
	std::vector<ActivationInterface*> AvailableActivations;
public:
	virtual std::vector<ActivationInterface*> &GetAvailableActivations() = 0;
	virtual void LoadActivationCore() = 0;
	virtual void LoadActivationPlugins() = 0;
	virtual void LoadActivationScripts() = 0;
};