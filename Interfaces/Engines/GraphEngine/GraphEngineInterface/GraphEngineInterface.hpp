#pragma once
#include "../Graph/Graph.hpp"

#include <vector>
#include <map>
#include <string>
#include <filesystem>

//TODO: change how loading AvailableNodes works were using too much memory storing instances of the nodes. maybe just store functions to create the nodes instead of the nodes themselves
class GraphEngineInterface: public EngineInterface {
protected:
	std::map<std::string,Graph*> Graphs;
	std::vector<SorterInterface*> AvailableSorters;
	ActivationEngineInterface* AE;
	LossEngineInterface* LE;
public:

	void SetDCEEngine(DynamicCodeExecutionEngineInterface* DCEE) override{
		this->DCEEngine = DCEE;
		#if defined(_MSC_VER)
			AE = DCEE->AddEngineInstance<ActivationEngineInterface>(DCEE->GetEngine("ActivationEngine.dll")->GetInstance<ActivationEngineInterface>());
		#elif defined(__GNUC__)
			AE = DCEE->AddEngineInstance<ActivationEngineInterface>(DCEE->GetEngine("libActivationEngine.so")->GetInstance<ActivationEngineInterface>());
		#endif
		AE->LoadActivationCore();
		AE->LoadActivationPlugins();
		AE->LoadActivationScripts();
		#if defined(_MSC_VER)
			LE = DCEE->AddEngineInstance<LossEngineInterface>(DCEE->GetEngine("LossEngine.dll")->GetInstance<LossEngineInterface>());
		#elif defined(__GNUC__)
			LE = DCEE->AddEngineInstance<LossEngineInterface>(DCEE->GetEngine("libLossEngine.so")->GetInstance<LossEngineInterface>());
		#endif
		LE->LoadLossCore();
		LE->LoadLossPlugins();
		LE->LoadLossScripts();
	}

	//getters
	std::map<std::string,Graph*> GetGraphs() {
		return Graphs;
	}

	void CreateGraph(std::string name) {
		Graph* graph = new Graph();
		graph->SetDCEE(DCEEngine);
		graph->SetAE(AE);
		graph->SetLE(LE);
		graph->SetName(name);
		Graphs[name] = graph;
	}

	virtual void LoadAvailableDataObjects() = 0;
	virtual void LoadAvailableNodes() = 0;
	virtual void LoadAvailableScriptNodes() = 0;
	virtual void LoadAvailableSorters() = 0;

	//getters
	std::vector<NodeInfo>& GetAvailableNodes() {
		return AvailableNodes;
	}

	std::vector<SorterInterface*>& GetAvailableSorters() {
		return AvailableSorters;
	}

	std::vector<ActivationInterface*>& GetAvailableActivations() {
		return AE->GetAvailableActivations();
	}

	std::vector<LossInterface*>& GetAvailableLosses() {
		return LE->GetAvailableLosses();
	}
};