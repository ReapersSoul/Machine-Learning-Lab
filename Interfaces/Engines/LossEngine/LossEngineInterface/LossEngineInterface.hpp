#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.hpp"
#include "../LossInterface/LossInterface.hpp"

class LossEngineInterface : public EngineInterface {
protected:
	std::vector<LossInterface*> LossActivations;
public:
	virtual std::vector<LossInterface*>& GetAvailableLosses() = 0;
	virtual void LoadLossCore() = 0;
	virtual void LoadLossPlugins() = 0;
	virtual void LoadLossScripts() = 0;
};

