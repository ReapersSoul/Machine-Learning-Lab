#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include "../Optimizers/OptimizerInterface/OptimizerInterface.hpp"

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