#include "GraphInterface.h"

void GraphInterface::Clear() {
	for (auto& Node : Nodes) {
		delete Node.second;
	}
	for (auto& EdgeInterface : Edges) {
		delete EdgeInterface.second;
	}
	Nodes.clear();
	Edges.clear();
	Name = "";
}

std::mutex& GraphInterface::GetMutex() {
	return GraphMutex;
}

void GraphInterface::SetName(std::string Name) {
	this->Name = Name;
}

std::string GraphInterface::GetName() {
	return Name;
}

void GraphInterface::SetDCEE(DynamicCodeExecutionEngineInterface* DCEE) {
	this->DCEE = DCEE;
}

void GraphInterface::SetAE(ActivationEngineInterface* AE) {
	this->AE = AE;
}

void GraphInterface::SetLE(LossEngineInterface* LE) {
	this->LE = LE;
}

EdgeInterface* GraphInterface::CreateEdge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO)
{
	std::string FirstType = Nodes[First]->GetOutputByUID(FirstIO)["TypeID"].get<std::string>();
	std::string SecondType = Nodes[Second]->GetInputByUID(SecondIO)["TypeID"].get<std::string>();

	if (FirstType != "Any" && SecondType != "Any") {
		if (FirstType != SecondType) {
			return nullptr;
		}
	}

	EdgeInterface* edge = new EdgeInterface();
	edge->SetFirst(First);
	edge->SetSecond(Second);
	edge->SetFirstIO(FirstIO);
	edge->SetSecondIO(SecondIO);
	edge->SetParent(this);
	int UID = 0;
	while (Edges.find(UID) != Edges.end()) {
		UID++;
	}
	edge->SetUID(UID);
	Edges[edge->GetUID()] = edge;
	return Edges[edge->GetUID()];
}

void GraphInterface::DeleteEdge(EdgeInterface* EdgeInterface)
{
	Edges.erase(EdgeInterface->GetUID());
}

void GraphInterface::DeleteEdge(unsigned int EdgeInterface) {
	Edges.erase(EdgeInterface);
}

int GraphInterface::AddNode(NodeInterface* Node) {
	int UID = 0;
	while (Nodes.find(UID) != Nodes.end()) {
		UID++;
	}
	Node->SetUID(UID);
	Node->SetParentGraph(this);
	Node->SetDCEE(DCEE);
	Node->SetAE(AE);
	Node->SetLE(LE);
	Node->Init();
	Nodes[UID] = Node;
	return UID;
}

void GraphInterface::DeleteNode(NodeInterface* Node) {
	Nodes.erase(Node->GetUID());
}

void GraphInterface::DeleteNode(unsigned int NodeUID) {
	Nodes.erase(NodeUID);
}

void GraphInterface::DeleteNode(int NodeUID) {
	Nodes[NodeUID]->~NodeInterface();
	Nodes.erase(NodeUID);
}

void GraphInterface::Process(bool DirectionForward, std::vector<int> SelectedNodes, std::vector<int> SelectedEdges) {
	ProcessingOrder PO = Sorter->SortGraph(this, DirectionForward);
	for (int i = 0; i < PO.Nodes.size(); i++)
	{
		for (int j = 0; j < PO.Nodes[i].size(); j++)
		{
			PO.Nodes[i][j]->ResetIO();
		}
	}

	for (int i = 0; i < PO.Nodes.size(); i++)
	{
		for (int j = 0; j < PO.Nodes[i].size(); j++) {
			PO.Nodes[i][j]->ClearAllOutputData();
			PO.Nodes[i][j]->ClearAllInputData();
		}
	}

	for (int i = 0; i < PO.Nodes.size(); i++)
	{
		for (int j = 0; j < PO.Nodes[i].size(); j++)
		{
			//if node is selected break
			bool selected = false;
			for (int k = 0; k < SelectedNodes.size(); k++)
			{
				if (PO.Nodes[i][j]->GetUID() == SelectedNodes[k]) {
					selected = true;
					break;
				}
			}
			if (selected) {
				printf("Proccesing Selected Node\n");
			}

			PO.Nodes[i][j]->Process(DirectionForward);
		}
		if (i < PO.Edges.size()) {
			for (int j = 0; j < PO.Edges[i].size(); j++)
			{
				//if edge is selected break
				bool selected = false;
				for (int k = 0; k < SelectedEdges.size(); k++)
				{
					if (PO.Edges[i][j]->GetUID() == SelectedEdges[k]) {
						selected = true;
						break;
					}
				}
				if (selected) {
					printf("Proccesing Selected Edge");
				}

				PO.Edges[i][j]->Process(DirectionForward);
			}
		}
	}
}

void GraphInterface::SetSorter(SorterInterface* Sorter) {
	this->Sorter = Sorter;
}

//getters
std::map<unsigned int, NodeInterface*>& GraphInterface::GetNodes() {
	return Nodes;
}

std::map<unsigned int, EdgeInterface*>& GraphInterface::GetEdges() {
	return Edges;
}

unsigned int GraphInterface::GetNextIOUID(unsigned int nodeID) {
	unsigned int UID = 0;
	while (IOUIDs.find(UID) != IOUIDs.end()) {
		UID++;
	}
	IOUIDs[UID] = nodeID;
	return UID;
}

void GraphInterface::RemoveIOUID(unsigned int UID) {
	IOUIDs.erase(UID);
}

unsigned int GraphInterface::GetIOAssociatedWith(unsigned int UID)
{
	return IOUIDs[UID];
}

nlohmann::json GraphInterface::Serialize() {
	nlohmann::json data;
	//graph name
	data["GraphName"] = Name;
	//edges
	data["Edges"] = nlohmann::json::array();
	for (auto& EdgeInterface : Edges) {
		data["Edges"].push_back(EdgeInterface.second->Serialize());
	}
	//nodes
	data["Nodes"] = nlohmann::json::array();
	for (auto& Node : Nodes) {
		data["Nodes"].push_back(Node.second->Serialize());
	}
	return data;
}

void GraphInterface::DeSerialize(nlohmann::json data, void* DCEE) {
	int offset = 0;
	Clear();
	//set DCEE
	this->DCEE = (DynamicCodeExecutionEngineInterface*)DCEE;
	//set AE
	this->AE = ((DynamicCodeExecutionEngineInterface*)DCEE)->GetEngineInstanceAs<ActivationEngineInterface>("ActivationEngine");
	//set LE
	this->LE = ((DynamicCodeExecutionEngineInterface*)DCEE)->GetEngineInstanceAs<LossEngineInterface>("LossEngine");
	//graph name
	Name = data["GraphName"];

	//edges
	for (int i = 0; i < data["Edges"].size(); i++) {
		EdgeInterface* Edge = new EdgeInterface();
		Edge->SetUID(data["Edges"][i]["UID"]);
		Edge->SetParent(this);
		Edge->DeSerialize(data["Edges"][i], DCEE);
		Edges[data["Edges"][i]["UID"]] = Edge;
	}
	//print all libs
	//for (auto& Lib : this->DCEE->GetCoreLibraries()) {
	//	printf("Core Lib: %s\n", Lib->GetPath().c_str());
	//}
	//for (auto& Lib : this->DCEE->GetLanguageLibraries()) {
	//	printf("Language Lib: %s\n", Lib->GetPath().c_str());
	//}
	//for (auto& Lib : this->DCEE->GetPluginLibraries()) {
	//	printf("Plugin Lib: %s\n", Lib->GetPath().c_str());
	//}
	//for (auto& Lib : this->DCEE->GetOtherLibraries()) {
	//	printf("Other Lib: %s\n", Lib->GetPath().c_str());
	//}

	//nodes
	for (int i = 0; i < data["Nodes"].size(); i++) {
		LibraryInterface * Lib = this->DCEE->GetOtherLib(data["Nodes"][i]["TypeID"].get<std::string>() + ".dll");
		if (Lib != nullptr) {
			NodeInterface* Node = Lib->GetInstance<NodeInterface>();
			Node->SetUID(data["Nodes"][i]["UID"]);
			Node->SetParentGraph(this);
			Node->DeSerialize(data["Nodes"][i], DCEE);
			Nodes[data["Nodes"][i]["UID"]] = Node;
		}
		else {
			printf("Library %s not found\n", data["Nodes"][i]["TypeID"].get<std::string>().c_str());
		}
	}


	return;
}