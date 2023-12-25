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
	NS_Node::Registrar NodeRegistrar;
	ActivationEngineInterface* AE;
	NS_Activation::Registrar* ActivationRegistrar;
	LossEngineInterface* LE;
	NS_Loss::Registrar* LossRegistrar;
public:

	void SetDCEEngine(DynamicCodeExecutionEngineInterface* DCEE) override{
		this->DCEEngine = DCEE;
		#if defined(_MSC_VER)
			AE = DCEE->AddEngineInstance<ActivationEngineInterface>(DCEE->GetEngine("ActivationEngine.dll")->GetInstance<ActivationEngineInterface>());
		#elif defined(__GNUC__)
			AE = DCEE->AddEngineInstance<ActivationEngineInterface>(DCEE->GetEngine("libActivationEngine.so")->GetInstance<ActivationEngineInterface>());
		#endif
		ActivationRegistrar=AE->GetRegistrar();
		AE->LoadActivationCore();
		AE->LoadActivationPlugins();
		AE->LoadActivationScripts();
		#if defined(_MSC_VER)
			LE = DCEE->AddEngineInstance<LossEngineInterface>(DCEE->GetEngine("LossEngine.dll")->GetInstance<LossEngineInterface>());
		#elif defined(__GNUC__)
			LE = DCEE->AddEngineInstance<LossEngineInterface>(DCEE->GetEngine("libLossEngine.so")->GetInstance<LossEngineInterface>());
		#endif
		LossRegistrar=LE->GetRegistrar();
		LE->LoadLossCore();
		LE->LoadLossPlugins();
		LE->LoadLossScripts();
	}

	//getters
	std::map<std::string,Graph*> GetGraphs() {
		return Graphs;
	}

	NS_Node::Registrar* GetNodeRegistrar() {
		return &NodeRegistrar;
	}

	ActivationEngineInterface* GetAE() {
		return AE;
	}

	NS_Activation::Registrar* GetActivationRegistrar() {
		return ActivationRegistrar;
	}

	LossEngineInterface* GetLE() {
		return LE;
	}

	NS_Loss::Registrar* GetLossRegistrar() {
		return LossRegistrar;
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
};