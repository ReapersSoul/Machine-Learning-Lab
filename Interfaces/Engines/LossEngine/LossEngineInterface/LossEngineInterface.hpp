#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.hpp"
#include "../LossInterface/LossInterface.hpp"

class LossEngineInterface : public EngineInterface {
public:
	virtual void LoadLossCore() = 0;
	virtual void LoadLossPlugins() = 0;
	virtual void LoadLossScripts() = 0;
	virtual NS_Loss::Registrar* GetRegistrar() = 0;
};

