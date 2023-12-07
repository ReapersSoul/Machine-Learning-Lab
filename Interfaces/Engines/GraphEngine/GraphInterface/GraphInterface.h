#pragma once
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.h"
#include "../../../SerializableInterface/SerializableInterface.h"
#include "../NodeInterface/NodeInterface.h"
#include "../EdgeInterface/EdgeInterface.h"
#include "../SorterInterface/SorterInterface.h"
#include "../../ActivationEngine/ActivationEngineInterface/ActivationEngineInterface.h"
#include "../../LossEngine/LossEngineInterface/LossEngineInterface.h"
#include <string>
#include <map>
#include <functional>

class GraphInterface : public SerializableInterface
{
protected:
	std::vector<unsigned int> UsedUIDs;
	std::map<unsigned int,unsigned int> IOUIDs;
	std::map<unsigned int, NodeInterface*> Nodes;
	std::map<unsigned int, EdgeInterface*> Edges;
	std::string Name;
	SorterInterface* Sorter;
	DynamicCodeExecutionEngineInterface* DCEE;
	ActivationEngineInterface* AE;
	LossEngineInterface* LE;
	//mutex for multithreading
	std::mutex GraphMutex;

public:
	
	std::mutex& GetMutex();

	void Clear();

	void SetName(std::string Name);

	std::string GetName();

	void SetDCEE(DynamicCodeExecutionEngineInterface* DCEE);

	void SetAE(ActivationEngineInterface* AE);

	void SetLE(LossEngineInterface* LE);

	EdgeInterface* CreateEdge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO);
	
	void DeleteEdge(EdgeInterface* EdgeInterface);

	void DeleteEdge(unsigned int EdgeInterface);

	int AddNode(NodeInterface* Node);

	void DeleteNode(NodeInterface* Node);

	void DeleteNode(unsigned int NodeUID);

	void ChangeNodeUID(unsigned int OldUID, unsigned int NewUID);

	void ChangeEdgeUID(unsigned int OldUID, unsigned int NewUID);

	void ChangeIOUID(unsigned int OldUID, unsigned int NewUID);

	void Process(bool DirectionForward, std::vector<unsigned int> SelectedNodes =std::vector<unsigned int>(), std::vector<unsigned int> SelectedEdges = std::vector<unsigned int>());

	void SetSorter(SorterInterface * Sorter);

	unsigned int GetNextIOUID(unsigned int);
	unsigned int GetNextUID();

	void RemoveIOUID(unsigned int UID);

	unsigned int GetIOAssociatedWith(unsigned int UID);

	//getters
	std::map<unsigned int, NodeInterface*>& GetNodes();

	std::map<unsigned int, EdgeInterface*>& GetEdges();

	std::string GetCurrentVersion();

	void UpdateSaveForCurrentVersion(std::string oldversion);

	nlohmann::json Serialize() override;
	void DeSerialize(nlohmann::json data, void* DCEE) override;
};