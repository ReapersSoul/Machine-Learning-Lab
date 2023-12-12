#pragma once
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.hpp"
#include "../../../SerializableInterface/SerializableInterface.hpp"
#include "../Edge/Edge.hpp"
#include "../NodeInterface/NodeInterface.hpp"
#include "../SorterInterface/SorterInterface.hpp"
#include "../../ActivationEngine/ActivationEngineInterface/ActivationEngineInterface.hpp"
#include "../../LossEngine/LossEngineInterface/LossEngineInterface.hpp"
#include <string>
#include <map>
#include <functional>

class Graph : public SerializableInterface
{
protected:
	std::vector<unsigned int> UsedUIDs;
	std::map<unsigned int, IO> Inputs;
	std::map<unsigned int, IO> Outputs;
	std::map<unsigned int, NS_Node::NodeInterface*> Nodes;
	std::map<unsigned int, Edge> Edges;
	std::string Name;
	NS_Sorter::SorterInterface* Sorter;
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

	Edge* CreateEdge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO);

	void DeleteEdge(unsigned int Edge);

	unsigned int CreateInput(unsigned int TypeID, std::function<void()> DrawFunction);

	void DeleteInput(unsigned int Input);

	unsigned int CreateOutput(unsigned int TypeID, std::function<void()> DrawFunction);

	void DeleteOutput(unsigned int Output);

	unsigned int AddNode(NS_Node::NodeInterface* Node);

	void DeleteNode(unsigned int NodeUID);

	void ChangeNodeUID(unsigned int OldUID, unsigned int NewUID);

	void ChangeEdgeUID(unsigned int OldUID, unsigned int NewUID);

	void ChangeInputUID(unsigned int OldUID, unsigned int NewUID);
	
	void ChangeOutputUID(unsigned int OldUID, unsigned int NewUID);

	void Process(bool DirectionForward, std::vector<unsigned int> SelectedNodes =std::vector<unsigned int>(), std::vector<unsigned int> SelectedEdges = std::vector<unsigned int>());

	void SetSorter(NS_Sorter::SorterInterface * Sorter);

	unsigned int GetNextUID();

	void RemoveIOUID(unsigned int UID);

	unsigned int GetIOAssociatedWith(unsigned int UID);

	//getters
	std::map<unsigned int, NS_Node::NodeInterface*>& GetNodes();

	std::map<unsigned int, Edge>& GetEdges();

	std::map<unsigned int, IO>& GetInputs();

	std::map<unsigned int, IO>& GetOutputs();

	NS_Node::NodeInterface* GetNodeByUID(unsigned int UID);

	Edge* GetEdgeByUID(unsigned int UID);

	IO* GetInputByUID(unsigned int UID);

	IO* GetOutputByUID(unsigned int UID);

	std::string GetCurrentVersion();

	std::vector<IO*> GetUnconnectedInputs();

	std::vector<IO*> GetUnconnectedOutputs();

	std::vector<IO*> GetInputNodeOutputs();

	std::vector<IO*> GetOutputNodeInputs();

	std::vector<IO*> GetUnconnectedInputsAndInputNodeOutputs();

	std::vector<IO*> GetUnconnectedOutputsAndOutputNodeInputs();

	void UpdateSaveForCurrentVersion(std::string oldversion);

	nlohmann::json Serialize() override;
	void DeSerialize(nlohmann::json data, void* DCEE) override;
};