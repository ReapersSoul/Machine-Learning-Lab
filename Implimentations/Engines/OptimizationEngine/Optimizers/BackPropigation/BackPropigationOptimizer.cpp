#include <OptimizerInterface.h>
#include <GraphInterface.h>

class BackPropigationOptimizer : public OptimizerInterface
{
	LossEngineInterface* LossEngine;
	LossInterface* Loss;

public:
	BackPropigationOptimizer() {
		Name = "BackPropigationOptimizer";
		SetType(Type::ForwardGradient);
	}

	void Optimize(GraphInterface* graph, std::vector<unsigned int> NodesToOptimize,std::vector<unsigned int> EdgesToOptimize) {
		graph->SetInputs(nlohmann::json());
		graph->Process(true);
		graph->ResetIOLocks();
		graph->Process(false);
	}

	double DetermineFitness(GraphInterface* graph) {
		//find loss node and get loss
		graph->Process(true);
		nlohmann::json output=graph->GetOutputDescriptors();

	}

	void ProvideForwardGradient(GraphInterface* graph) {
		
	}
};