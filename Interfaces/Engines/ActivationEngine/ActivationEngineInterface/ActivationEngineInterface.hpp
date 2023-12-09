#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.hpp"
#include "../ActivationInterface/ActivationInterface.hpp"

class ActivationEngineInterface : public EngineInterface {
protected:
	std::vector<ActivationInterface*> AvailableActivations;
public:
	virtual std::vector<ActivationInterface*> &GetAvailableActivations() = 0;
	virtual void LoadActivationCore() = 0;
	virtual void LoadActivationPlugins() = 0;
	virtual void LoadActivationScripts() = 0;
};