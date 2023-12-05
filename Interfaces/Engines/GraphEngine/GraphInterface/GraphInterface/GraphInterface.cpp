#include "GraphInterface.h"

nlohmann::json GraphInterface::GetInputDescriptors()
{
	nlohmann::json Inputs= nlohmann::json::array();
	//find input nodes output io and the unconeccted input io
	for (auto& Node : Nodes) {
		if (Node.second->InputCount() == 0)
			Inputs.push_back(Node.second->GetOutputDescriptors());
		if (Node.second->InputCount() > 0) {
			//get the InputEdges
			std::vector<EdgeInterface*> InputEdges = Node.second->GetInputEdges();
			//collect the ids of the IOUID's
			std::vector<unsigned int> InputIOUIDs;
			for (auto& InputEdge : InputEdges) {
				InputIOUIDs.push_back(InputEdge->GetSecondIO());
			}
			for (unsigned int i = 0; i < Node.second->InputCount(); i++) {
				bool isConnected = false;
				nlohmann::json InputDescriptors = Node.second->GetInputDescriptors();
				for (auto& InputIOUID : InputIOUIDs) {
					if (InputDescriptors[i]["IOUID"] == InputIOUID) {
						isConnected = true;
						break;
					}
				}
				if (!isConnected)
					Inputs.push_back(InputDescriptors[i]);
			}
		}
	}
	return Inputs;
}

nlohmann::json GraphInterface::GetOutputDescriptors()
{
	nlohmann::json Outputs = nlohmann::json::array();
	for (auto& Node : Nodes) {
		if (Node.second->OutputCount() == 0)
			Outputs.push_back(Node.second->GetOutputDescriptors());
		if (Node.second->OutputCount() > 0) {
			//get the OutputEdges
			std::vector<EdgeInterface*> OutputEdges = Node.second->GetOutputEdges();
			//collect the ids of the IOUID's
			std::vector<unsigned int> OutputIOUIDs;
			for (auto& OutputEdge : OutputEdges) {
				OutputIOUIDs.push_back(OutputEdge->GetFirstIO());
			}
			for (unsigned int i = 0; i < Node.second->OutputCount(); i++) {
				bool isConnected = false;
				nlohmann::json OutputDescriptors = Node.second->GetOutputDescriptors();
				for (auto& OutputIOUID : OutputIOUIDs) {
					if (OutputDescriptors[i]["IOUID"] == OutputIOUID) {
						isConnected = true;
						break;
					}
				}
				if (!isConnected)
					Outputs.push_back(OutputDescriptors[i]);
			}
		}
	}
	return Outputs;
}

void GraphInterface::SetInputs(nlohmann::json Inputs)
{
	//find input nodes output io and the unconeccted input io and set their data 
	for (auto& Node : Nodes) {
		bool HasInput = false;
		for (auto& Input : Inputs) {
			if (Node.second->HasInput(Input["UID"])) {
				Node.second->GetInputByUID(Input["UID"])["Data"] = Input["Data"];
				Node.second->LockInput(Input["UID"]);
			}
			if (Node.second->HasOutput(Input["UID"])) {
				Node.second->GetOutputByUID(Input["UID"])["Data"] = Input["Data"];
				Node.second->LockOutput(Input["UID"]);
			}
		}		
	}
}

void GraphInterface::SetOutputs(nlohmann::json Outputs)
{
	//find output nodes input io and the unconeccted output io and set their data 
	for (auto& Node : Nodes) {
		bool HasOutput = false;
		for (auto& Output : Outputs) {
			if (Node.second->HasOutput(Output["UID"])) {
				Node.second->GetOutputByUID(Output["UID"])["Data"] = Output["Data"];
				Node.second->LockOutput(Output["UID"]);
			}
			if (Node.second->HasInput(Output["UID"])) {
				Node.second->GetInputByUID(Output["UID"])["Data"] = Output["Data"];
				Node.second->LockInput(Output["UID"]);
			}
		}
	}
}

void GraphInterface::LockInput(unsigned int UID) {
	for (auto& Node : Nodes) {
		if (Node.second->HasInput(UID)) {
			Node.second->LockInput(UID);
			return;
		}
	}
}
void GraphInterface::LockOutput(unsigned int UID) {
	for (auto& Node : Nodes) {
		if (Node.second->HasOutput(UID)) {
			Node.second->LockOutput(UID);
			return;
		}
	}
}
void GraphInterface::UnlockInput(unsigned int UID) {
	for (auto& Node : Nodes) {
		if (Node.second->HasInput(UID)) {
			Node.second->UnlockInput(UID);
			return;
		}
	}
}
void GraphInterface::UnlockOutput(unsigned int UID) {
	for (auto& Node : Nodes) {
		if (Node.second->HasOutput(UID)) {
			Node.second->UnlockOutput(UID);
			return;
		}
	}
}
bool GraphInterface::InputIsLocked(unsigned int UID) {
	for (auto& Node : Nodes) {
		if (Node.second->HasInput(UID)) {
			return Node.second->InputIsLocked(UID);
		}
	}
	return false;
}
bool GraphInterface::OutputIsLocked(unsigned int UID) {
	for (auto& Node : Nodes) {
		if (Node.second->HasOutput(UID)) {
			return Node.second->OutputIsLocked(UID);
		}
	}
	return false;
}

void GraphInterface::LockInputs(std::vector<unsigned int> UIDs) {
	for (auto& Node : Nodes) {
		for (auto& UID : UIDs) {
			if (Node.second->HasInput(UID)) {
				Node.second->LockInput(UID);
			}
		}
	}
}
void GraphInterface::LockOutputs(std::vector<unsigned int> UIDs) {
	for (auto& Node : Nodes) {
		for (auto& UID : UIDs) {
			if (Node.second->HasOutput(UID)) {
				Node.second->LockOutput(UID);
			}
		}
	}
}
void GraphInterface::UnlockInputs(std::vector<unsigned int> UIDs) {
	for (auto& Node : Nodes) {
		for (auto& UID : UIDs) {
			if (Node.second->HasInput(UID)) {
				Node.second->UnlockInput(UID);
			}
		}
	}
}
void GraphInterface::UnlockOutputs(std::vector<unsigned int> UIDs) {
	for (auto& Node : Nodes) {
		for (auto& UID : UIDs) {
			if (Node.second->HasOutput(UID)) {
				Node.second->UnlockOutput(UID);
			}
		}
	}
}
bool GraphInterface::InputsAreLocked(std::vector<unsigned int> UIDs) {
	for (auto& Node : Nodes) {
		for (auto& UID : UIDs) {
			if (Node.second->HasInput(UID)) {
				if (!Node.second->InputIsLocked(UID)) {
					return false;
				}
			}
		}
	}
	return true;
}
bool GraphInterface::OutputsAreLocked(std::vector<unsigned int> UIDs) {
	for (auto& Node : Nodes) {
		for (auto& UID : UIDs) {
			if (Node.second->HasOutput(UID)) {
				if (!Node.second->OutputIsLocked(UID)) {
					return false;
				}
			}
		}
	}
	return true;
}

void GraphInterface::ResetIOLocks() {
	for (auto& Node : Nodes) {
		Node.second->ResetIOLocks();
	}
}

GraphInterface* GraphInterface::SubGraph(std::vector<unsigned int> SelectedNodes, std::vector<unsigned int> SelectedEdges)
{
	GraphInterface* SubGraph = new GraphInterface();
	SubGraph->SetName(Name + "SubGraph");
	for (auto& Node : Nodes) {
		if (std::find(SelectedNodes.begin(), SelectedNodes.end(), Node.first) != SelectedNodes.end()) {
			SubGraph->AddNode(Node.second->Clone());
		}
	}
	for (auto& Edge : Edges) {
		if (std::find(SelectedEdges.begin(), SelectedEdges.end(), Edge.first) != SelectedEdges.end()) {
			SubGraph->AddEdge(Edge.second->Clone());
		}
	}
	return SubGraph;
}

void GraphInterface::ReplaceSubGraph(GraphInterface* SubGraph)
{
	//if it has the nodes and edges, replace them
	for (auto& Node : SubGraph->GetNodes()) {
		if (HasNode(Node.first)) {
			DeleteNode(Node.first);
		}
		AddNode(Node.second);
	}
	for (auto& Edge : SubGraph->GetEdges()) {
		if (HasEdge(Edge.first)) {
			DeleteEdge(Edge.first);
		}
		AddEdge(Edge.second);
	}
}

void GraphInterface::AddEdge(EdgeInterface* EdgeInterface)
{
	Edges[EdgeInterface->GetUID()] = EdgeInterface;
}

bool GraphInterface::HasNode(unsigned int NodeUID)
{
	return Nodes.find(NodeUID) != Nodes.end();
}

bool GraphInterface::HasEdge(unsigned int EdgeUID)
{
	return Edges.find(EdgeUID) != Edges.end();
}

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

void GraphInterface::DeleteNode(NodeInterface* Node) {
	Nodes.erase(Node->GetUID());
}

void GraphInterface::DeleteNode(unsigned int NodeUID) {
	Nodes[NodeUID]->~NodeInterface();
	Nodes.erase(NodeUID);
}

void GraphInterface::ChangeNodeUID(unsigned int OldUID, unsigned int NewUID)
{
	NodeInterface* Node = Nodes[OldUID];
	for (auto& Edge : Edges) {
		if (Edge.second->GetFirst()->GetUID() == OldUID) {
			Edge.second->SetFirst(NewUID);
		}
		if (Edge.second->GetSecond()->GetUID() == OldUID) {
			Edge.second->SetSecond(NewUID);
		}
	}
	for (auto& IOUID : IOUIDs) {
		if (IOUID.second == OldUID) {
			IOUID.second = NewUID;
		}
	}

	Nodes.erase(OldUID);
	Nodes[NewUID] = Node;
	Node->SetUID(NewUID);
}

void GraphInterface::ChangeEdgeUID(unsigned int OldUID, unsigned int NewUID)
{
	EdgeInterface* Edge = Edges[OldUID];
	Edges.erase(OldUID);
	Edges[NewUID] = Edge;
	Edge->SetUID(NewUID);
}

void GraphInterface::ChangeIOUID(unsigned int OldUID, unsigned int NewUID)
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

void GraphInterface::Process(bool DirectionForward, std::vector<unsigned int> SelectedNodes, std::vector<unsigned int> SelectedEdges) {
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

void GraphInterface::Train()
{
	ProcessingOrder PO = Sorter->SortGraph(this, true);
	//node, optimizer for layer
	std::map<unsigned int, Optimizer*> Optimizers;
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
			PO.Nodes[i][j]->Process(DirectionForward);
		}
		if (i < PO.Edges.size()) {
			for (int j = 0; j < PO.Edges[i].size(); j++)
			{
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
	unsigned int UID = GetNextUID();
	IOUIDs[UID] = nodeID;
	return UID;
}

unsigned int GraphInterface::GetNextUID()
{
	unsigned int UID = 0;
	while (std::find(UsedUIDs.begin(), UsedUIDs.end(), UID) != UsedUIDs.end()) {
		UID++;
	}
	UsedUIDs.push_back(UID);
	return UID;
}

void GraphInterface::RemoveIOUID(unsigned int UID) {
	IOUIDs.erase(UID);
}

unsigned int GraphInterface::GetIOAssociatedWith(unsigned int UID)
{
	return IOUIDs[UID];
}

std::string GraphInterface::GetCurrentVersion() {
	return "0.0.1";
}

void GraphInterface::UpdateSaveForCurrentVersion(std::string oldversion)
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

nlohmann::json GraphInterface::Serialize() {
	nlohmann::json data;
	data["Version"] = GetCurrentVersion();
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
	//check version
	//if version doesnt exist set to 0.0.0
	if (data["Version"].is_null()) {
		data["Version"] = "0.0.0";
	}

	
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

	UpdateSaveForCurrentVersion(data["Version"]);
	return;
}