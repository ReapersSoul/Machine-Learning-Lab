#pragma once
#include "../../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.h"
#include "../../../SerializationEngine/SerializableInterface/SerializableInterface/SerializableInterface.h"
#include "../../Nodes/NodeInterface/NodeInterface.h"
#include "../../EdgeInterface/EdgeInterface/EdgeInterface.h"
#include "../../Sorters/SorterInterface/SorterInterface.h"
#include "../../../ActivationEngine/ActivationEngineInterface/ActivationEngineInterface.h"
#include "../../../LossEngine/LossEngineInterface/LossEngineInterface.h"
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
	
	nlohmann::json GetInputDescriptors();
	nlohmann::json GetOutputDescriptors();

	void SetInputs(nlohmann::json Inputs);
	void SetOutputs(nlohmann::json Outputs);

	void LockInput(unsigned int UID);
	void LockOutput(unsigned int UID);
	void UnlockInput(unsigned int UID);
	void UnlockOutput(unsigned int UID);
	bool InputIsLocked(unsigned int UID);
	bool OutputIsLocked(unsigned int UID);

	void LockInputs(std::vector<unsigned int> UIDs);
	void LockOutputs(std::vector<unsigned int> UIDs);
	void UnlockInputs(std::vector<unsigned int> UIDs);
	void UnlockOutputs(std::vector<unsigned int> UIDs);
	bool InputsAreLocked(std::vector<unsigned int> UIDs);
	bool OutputsAreLocked(std::vector<unsigned int> UIDs);

	void ResetIOLocks();

	std::mutex& GetMutex();

	void Clear();

	void SetName(std::string Name);

	std::string GetName();

	void SetDCEE(DynamicCodeExecutionEngineInterface* DCEE);

	void SetAE(ActivationEngineInterface* AE);

	void SetLE(LossEngineInterface* LE);

	GraphInterface * SubGraph(std::vector<unsigned int> SelectedNodes = std::vector<unsigned int>(), std::vector<unsigned int> SelectedEdges = std::vector<unsigned int>());
	void ReplaceSubGraph(GraphInterface* SubGraph);

	EdgeInterface* CreateEdge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO);

	void AddEdge(EdgeInterface* EdgeInterface);
	
	void DeleteEdge(EdgeInterface* EdgeInterface);

	void DeleteEdge(unsigned int EdgeInterface);

	int AddNode(NodeInterface* Node);

	void DeleteNode(NodeInterface* Node);

	void DeleteNode(unsigned int NodeUID);

	bool HasNode(unsigned int NodeUID);

	bool HasEdge(unsigned int EdgeUID);

	void ChangeNodeUID(unsigned int OldUID, unsigned int NewUID);

	void ChangeEdgeUID(unsigned int OldUID, unsigned int NewUID);

	void ChangeIOUID(unsigned int OldUID, unsigned int NewUID);

	void Process(bool DirectionForward, std::vector<unsigned int> SelectedNodes =std::vector<unsigned int>(), std::vector<unsigned int> SelectedEdges = std::vector<unsigned int>());

	void Train();

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