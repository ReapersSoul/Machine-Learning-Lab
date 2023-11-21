#pragma once
#include "../../EngineInterface/EngineInterface.h"
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.h"
#include "../Losses/LossInterface/LossInterface.h"

class LossEngineInterface : public EngineInterface {
protected:
	std::vector<LossInterface*> LossActivations;
public:
	virtual std::vector<LossInterface*>& GetAvailableLosses() = 0;
	virtual void LoadLossCore() = 0;
	virtual void LoadLossPlugins() = 0;
	virtual void LoadLossScripts() = 0;
};

