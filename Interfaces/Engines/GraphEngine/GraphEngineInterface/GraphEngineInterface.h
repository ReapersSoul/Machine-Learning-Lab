#pragma once
#include "../GraphInterface/GraphInterface.h"

#include <vector>
#include <map>
#include <string>
#include <filesystem>

struct NodeInfo {
	std::string TypeID;
	std::function<NodeInterface*()> CreateNode;

};

//TODO: change how loading AvailableNodes works were using too much memory storing instances of the nodes. maybe just store functions to create the nodes instead of the nodes themselves
class GraphEngineInterface: public EngineInterface {
protected:
	std::map<std::string,GraphInterface*> Graphs;
	std::vector<NodeInfo> AvailableNodes;
	std::vector<NodeIOInterface*> AvailableNodeIOs;
	std::vector<SorterInterface*> AvailableSorters;
	ActivationEngineInterface* AE;
	LossEngineInterface* LE;
public:

	void SetDCEEngine(DynamicCodeExecutionEngineInterface* DCEE) override{
		this->DCEEngine = DCEE;
		AE = DCEE->AddEngineInstance<ActivationEngineInterface>(DCEE->GetEngine("ActivationEngine.dll")->GetInstance<ActivationEngineInterface>());
		AE->LoadActivationCore();
		AE->LoadActivationPlugins();
		AE->LoadActivationScripts();
		LE = DCEE->AddEngineInstance<LossEngineInterface>(DCEE->GetEngine("LossEngine.dll")->GetInstance<LossEngineInterface>());
		LE->LoadLossCore();
		LE->LoadLossPlugins();
		LE->LoadLossScripts();
	}

	//getters
	std::map<std::string,GraphInterface*> GetGraphs() {
		return Graphs;
	}

	void CreateGraph(std::string name) {
		GraphInterface* graph = new GraphInterface();
		graph->SetDCEE(DCEEngine);
		graph->SetAE(AE);
		graph->SetLE(LE);
		graph->SetName(name);
		Graphs[name] = graph;
	}

	virtual void LoadAvailableNodes() = 0;
	virtual void LoadAvailableScriptNodes() = 0;
	virtual void LoadAvailableNodeIOs() = 0;
	virtual void LoadAvailableSorters() = 0;

	//getters
	std::vector<NodeInfo>& GetAvailableNodes() {
		return AvailableNodes;
	}

	std::vector<NodeIOInterface*>& GetAvailableNodeIOs() {
		return AvailableNodeIOs;
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