#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.hpp"
#include "../ActivationInterface/ActivationInterface.hpp"


class ActivationEngineInterface : public EngineInterface {
protected:
	NS_Activation::Registrar Registrar;
public:
	virtual void LoadActivationCore() = 0;
	virtual void LoadActivationPlugins() = 0;
	virtual void LoadActivationScripts() = 0;
	NS_Activation::Registrar* GetRegistrar() {
		return &Registrar;
	}
};