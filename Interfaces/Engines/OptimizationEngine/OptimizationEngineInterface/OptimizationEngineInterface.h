#pragma once
#include "../../EngineInterface/EngineInterface.h"
#include "../Optimizers/OptimizerInterface/OptimizerInterface.h"

class OptimizationEngineInterface : public EngineInterface
{
protected:
		std::vector<OptimizerInterface*> AvailableOptimizers;
public:
		virtual std::vector<OptimizerInterface*>& GetAvailableOptimizers() = 0;
		virtual void LoadOptimizerCore() = 0;
		virtual void LoadOptimizerPlugins() = 0;
		virtual void LoadOptimizerScripts() = 0;
};