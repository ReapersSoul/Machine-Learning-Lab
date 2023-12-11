#include "Graph.hpp"

void Graph::Clear() {
	for (auto& Node : Nodes) {
		delete Node.second;
	}
	Nodes.clear();
	Edges.clear();
}

std::mutex& Graph::GetMutex() {
	return GraphMutex;
}

void Graph::SetName(std::string Name) {
	this->Name = Name;
}

std::string Graph::GetName() {
	return Name;
}

void Graph::SetDCEE(DynamicCodeExecutionEngineInterface* DCEE) {
	this->DCEE = DCEE;
}

void Graph::SetAE(ActivationEngineInterface* AE) {
	this->AE = AE;
}

void Graph::SetLE(LossEngineInterface* LE) {
	this->LE = LE;
}

Edge* Graph::CreateEdge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO)
{
	unsigned int FirstType = Nodes[First]->GetOutputByUID(FirstIO)->GetTypeID();
	unsigned int SecondType = Nodes[Second]->GetInputByUID(SecondIO)->GetTypeID();

	if (FirstType != DataObjectInterface::GetTypeID("Any") || SecondType != DataObjectInterface::GetTypeID("Any")) {
		if (FirstType != SecondType) {
			return nullptr;
		}
	}

	Nodes[Second]->GetInputByUID(SecondIO)->AddData(Nodes[First]->GetOutputByUID(FirstIO)->GetData());

	Edge edge;
	edge.SetFirst(First);
	edge.SetSecond(Second);
	edge.SetFirstIO(FirstIO);
	edge.SetSecondIO(SecondIO);
	edge.SetParent(this);
	int UID = 0;
	while (Edges.find(UID) != Edges.end()) {
		UID++;
	}
	edge.SetUID(UID);
	Edges[edge.GetUID()] = edge;
	return &Edges[edge.GetUID()];
}

void Graph::DeleteEdge(unsigned int Edge) {
	Edges.erase(Edge);
}

int Graph::AddNode(NodeInterface* Node) {
	unsigned int UID = GetNextUID();
	Node->SetUID(UID);
	Node->SetParentGraph(this);
	Node->SetDCEE(DCEE);
	Node->SetAE(AE);
	Node->SetLE(LE);
	Node->Init();
	Nodes[UID] = Node;
	return UID;
}

void Graph::DeleteNode(unsigned int NodeUID) {
	Nodes[NodeUID]->~NodeInterface();
	Nodes.erase(NodeUID);
}

void Graph::ChangeNodeUID(unsigned int OldUID, unsigned int NewUID)
{
	NodeInterface* Node = Nodes[OldUID];
	for (auto& Edge : Edges) {
		if (Edge.second.GetFirst()->GetUID() == OldUID) {
			Edge.second.SetFirst(NewUID);
		}
		if (Edge.second.GetSecond()->GetUID() == OldUID) {
			Edge.second.SetSecond(NewUID);
		}
	}

	Nodes.erase(OldUID);
	Nodes[NewUID] = Node;
	Node->SetUID(NewUID);
}

void Graph::ChangeEdgeUID(unsigned int OldUID, unsigned int NewUID)
{
	Edge Edge = Edges[OldUID];
	Edges.erase(OldUID);
	Edges[NewUID] = Edge;
	Edge.SetUID(NewUID);
}

void Graph::ChangeIOUID(unsigned int OldUID, unsigned int NewUID)
{
	unsigned int NodeUID= IOUIDs[OldUID];
	for (auto& Node : Nodes) {
		if (Node.second->HasInput(OldUID) || Node.second->HasOutput(OldUID)) {
			Node.second->ChangeIOUID(OldUID, NewUID);
		}
	}
	IOUIDs.erase(OldUID);
	IOUIDs[NewUID] = NodeUID;
}

void Graph::Process(bool DirectionForward, std::vector<unsigned int> SelectedNodes, std::vector<unsigned int> SelectedEdges) {
	ProcessingOrder PO = Sorter->SortGraph(this, DirectionForward);

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
	}
}

void Graph::SetSorter(SorterInterface* Sorter) {
	this->Sorter = Sorter;
}

//getters
std::map<unsigned int, NodeInterface*>& Graph::GetNodes() {
	return Nodes;
}

std::map<unsigned int, Edge>& Graph::GetEdges() {
	return Edges;
}

unsigned int Graph::GetNextIOUID(unsigned int nodeID) {
	unsigned int UID = GetNextUID();
	IOUIDs[UID] = nodeID;
	return UID;
}

unsigned int Graph::GetNextUID()
{
	unsigned int UID = 0;
	while (std::find(UsedUIDs.begin(), UsedUIDs.end(), UID) != UsedUIDs.end()) {
		UID++;
	}
	UsedUIDs.push_back(UID);
	return UID;
}

void Graph::RemoveIOUID(unsigned int UID) {
	IOUIDs.erase(UID);
}

unsigned int Graph::GetIOAssociatedWith(unsigned int UID)
{
	return IOUIDs[UID];
}

std::string Graph::GetCurrentVersion() {
	return "0.0.1";
}

void Graph::UpdateSaveForCurrentVersion(std::string oldversion)
{
	if (oldversion == GetCurrentVersion()) {
		return;
	}

	if (oldversion == "0.0.0") {
		int starting_uid = Edges.size();

		std::map<unsigned int, unsigned int> oldIOUID = IOUIDs;

		for (auto& IO : oldIOUID) {
			ChangeIOUID(IO.first, starting_uid);
			starting_uid++;
		}

		std::vector<NodeInterface*> nNodes;
		for (auto& Node : this->Nodes) {
			nNodes.push_back(Node.second);
		}

		//update to 0.0.1
		for (auto& Node : nNodes) {
			ChangeNodeUID(Node->GetUID(), starting_uid);
			starting_uid++;
		}


	}
}

nlohmann::json Graph::Serialize() {
	nlohmann::json data;
	data["Version"] = GetCurrentVersion();
	//graph name
	data["GraphName"] = Name;
	//edges
	data["Edges"] = nlohmann::json::array();
	for (auto& Edge : Edges) {
		data["Edges"].push_back(Edge.second.Serialize());
	}
	//nodes
	data["Nodes"] = nlohmann::json::array();
	for (auto& Node : Nodes) {
		data["Nodes"].push_back(Node.second->Serialize());
	}
	return data;
}

void Graph::DeSerialize(nlohmann::json data, void* DCEE) {
	int offset = 0;
	Clear();
	//set DCEE
	this->DCEE = (DynamicCodeExecutionEngineInterface*)DCEE;
	//set AE
	this->AE = ((DynamicCodeExecutionEngineInterface*)DCEE)->GetEngineInstanceAs<ActivationEngineInterface>("ActivationEngine");
	//set LE
	this->LE = ((DynamicCodeExecutionEngineInterface*)DCEE)->GetEngineInstanceAs<LossEngineInterface>("LossEngine");
	//check version
	//if version doesnt exist set to 0.0.0
	if (data["Version"].is_null()) {
		data["Version"] = "0.0.0";
	}

	
	//graph name
	Name = data["GraphName"];

	//edges
	for (int i = 0; i < data["Edges"].size(); i++) {
		Edge Edge;
		Edge.SetUID(data["Edges"][i]["UID"]);
		Edge.SetParent(this);
		Edge.DeSerialize(data["Edges"][i], DCEE);
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
		#if defined(_MSC_VER)
			LibraryInterface * Lib = this->DCEE->GetOtherLib(data["Nodes"][i]["TypeID"].get<std::string>() + ".dll");
		#elif defined(__GNUC__)
			LibraryInterface * Lib = this->DCEE->GetOtherLib("lib" + data["Nodes"][i]["TypeID"].get<std::string>() + ".so");
		#endif
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

	UpdateSaveForCurrentVersion(data["Version"]);
	return;
}