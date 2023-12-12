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

	if (FirstType != NS_DataObject::GetTypeID("Any") || SecondType != NS_DataObject::GetTypeID("Any")) {
		if (FirstType != SecondType) {
			return nullptr;
		}
	}
	for (auto& dataObj : Nodes[First]->GetOutputByUID(FirstIO)->GetData()) {
		Nodes[Second]->GetInputByUID(SecondIO)->AddData(dataObj);
	}

	Edge edge;
	edge.SetFirst(First);
	edge.SetSecond(Second);
	edge.SetFirstIO(FirstIO);
	edge.SetSecondIO(SecondIO);
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

unsigned int Graph::CreateInput(unsigned int TypeID, std::function<void()> DrawFunction) {
	IO Input;
	Input.SetTypeID(TypeID);
	Input.SetDrawFunction(DrawFunction);
	int UID = 0;
	while (Inputs.find(UID) != Inputs.end()) {
		UID++;
	}
	Input.SetUID(UID);
	Inputs[UID] = Input;
	return UID;
}

void Graph::DeleteInput(unsigned int Input) {
	Inputs.erase(Input);
}

unsigned int Graph::CreateOutput(unsigned int TypeID, std::function<void()> DrawFunction) {
	IO Output;
	Output.SetTypeID(TypeID);
	Output.SetDrawFunction(DrawFunction);
	int UID = 0;
	while (Outputs.find(UID) != Outputs.end()) {
		UID++;
	}
	Output.SetUID(UID);
	Outputs[UID] = Output;
	return UID;
}

void Graph::DeleteOutput(unsigned int Output) {
	Outputs.erase(Output);
}

unsigned int Graph::AddNode(NS_Node::NodeInterface* Node) {
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
	NS_Node::NodeInterface* Node = Nodes[OldUID];
	for (auto& Edge : Edges) {
		if (Edge.second.GetFirst() == OldUID) {
			Edge.second.SetFirst(NewUID);
		}
		if (Edge.second.GetSecond() == OldUID) {
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

void Graph::ChangeInputUID(unsigned int OldUID, unsigned int NewUID)
{
	auto it = std::find_if(Inputs.begin(), Inputs.end(), [OldUID](std::pair<unsigned int, IO> Input) {return Input.second.GetUID() == OldUID; });
	if (it == Inputs.end()) {
		throw std::runtime_error("Input not found");
	}
	unsigned int NodeUID= it->second.GetParentNodeUID();
	Nodes[NodeUID]->ChangeInputUID(OldUID, NewUID);
}

void Graph::ChangeOutputUID(unsigned int OldUID, unsigned int NewUID)
{
	auto it = std::find_if(Outputs.begin(), Outputs.end(), [OldUID](std::pair<unsigned int, IO> Output) {return Output.second.GetUID() == OldUID; });
	if (it == Outputs.end()) {
		throw std::runtime_error("Output not found");
	}
	unsigned int NodeUID = it->second.GetParentNodeUID();
	Nodes[NodeUID]->ChangeOutputUID(OldUID, NewUID);
}

void Graph::Process(bool DirectionForward, std::vector<unsigned int> SelectedNodes, std::vector<unsigned int> SelectedEdges) {
	NS_Sorter::ProcessingOrder PO = Sorter->SortGraph(this, DirectionForward);

	for (int i = 0; i < PO.Nodes.size(); i++)
	{
		for (int j = 0; j < PO.Nodes[i].size(); j++)
		{
			PO.Nodes[i][j]->ResetIO();
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
	}
}

void Graph::SetSorter(NS_Sorter::SorterInterface* Sorter) {
	this->Sorter = Sorter;
}

//getters
std::map<unsigned int, NS_Node::NodeInterface*>& Graph::GetNodes() {
	return Nodes;
}

std::map<unsigned int, Edge>& Graph::GetEdges() {
	return Edges;
}

std::map<unsigned int, IO>& Graph::GetInputs() {
	return Inputs;
}

std::map<unsigned int, IO>& Graph::GetOutputs() {
	return Outputs;
}

NS_Node::NodeInterface* Graph::GetNodeByUID(unsigned int UID) {
	return Nodes[UID];
}

Edge* Graph::GetEdgeByUID(unsigned int UID) {
	return &Edges[UID];
}

IO* Graph::GetInputByUID(unsigned int UID) {
	return &Inputs[UID];
}

IO* Graph::GetOutputByUID(unsigned int UID) {
	return &Outputs[UID];
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

std::string Graph::GetCurrentVersion() {
	return "0.0.1";
}

std::vector<IO*> Graph::GetUnconnectedInputs() {
	std::vector<IO*> UnconnectedInputs;
	for (auto& Input : Inputs) {
		bool found = false;
		for (auto& Edge : Edges) {
			if (Edge.second.GetSecondIO() == Input.second.GetUID()) {
				found = true;
				break;
			}
		}
		if (!found) {
			UnconnectedInputs.push_back(&Input.second);
		}
	}
	return UnconnectedInputs;
}

std::vector<IO*> Graph::GetUnconnectedOutputs() {
	std::vector<IO*> UnconnectedOutputs;
	for (auto& Output : Outputs) {
		bool found = false;
		for (auto& Edge : Edges) {
			if (Edge.second.GetFirstIO() == Output.second.GetUID()) {
				found = true;
				break;
			}
		}
		if (!found) {
			UnconnectedOutputs.push_back(&Output.second);
		}
	}
	return UnconnectedOutputs;
}

std::vector<IO*> Graph::GetInputNodeOutputs() {
	std::vector<IO*> InputNodeOutputs;
	for (auto& node : Nodes) {
		if (node.second->GetInputs().size() == 0) {
			for (auto& output : node.second->GetOutputs()) {
				InputNodeOutputs.push_back(output);
			}
		}
	}
	return InputNodeOutputs;
}

std::vector<IO*> Graph::GetOutputNodeInputs() {
	std::vector<IO*> OutputNodeInputs;
	for (auto& node : Nodes) {
		if (node.second->GetOutputs().size() == 0) {
			for (auto& input : node.second->GetInputs()) {
				OutputNodeInputs.push_back(input);
			}
		}
	}
	return OutputNodeInputs;
}

std::vector<IO*> Graph::GetUnconnectedInputsAndInputNodeOutputs() {
	std::vector<IO*> UnconnectedInputsAndInputNodeOutputs;
	std::vector<IO*> UnconnectedInputs = GetUnconnectedInputs();
	std::vector<IO*> InputNodeOutputs = GetInputNodeOutputs();
	for (auto& Input : UnconnectedInputs) {
		UnconnectedInputsAndInputNodeOutputs.push_back(Input);
	}
	for (auto& Input : InputNodeOutputs) {
		UnconnectedInputsAndInputNodeOutputs.push_back(Input);
	}
	return UnconnectedInputsAndInputNodeOutputs;
}

std::vector<IO*> Graph::GetUnconnectedOutputsAndOutputNodeInputs() {
	std::vector<IO*> UnconnectedOutputsAndOutputNodeInputs;
	std::vector<IO*> UnconnectedOutputs = GetUnconnectedOutputs();
	std::vector<IO*> OutputNodeInputs = GetOutputNodeInputs();
	for (auto& Output : UnconnectedOutputs) {
		UnconnectedOutputsAndOutputNodeInputs.push_back(Output);
	}
	for (auto& Output : OutputNodeInputs) {
		UnconnectedOutputsAndOutputNodeInputs.push_back(Output);
	}
	return UnconnectedOutputsAndOutputNodeInputs;
}

void Graph::UpdateSaveForCurrentVersion(std::string oldversion)
{
	if (oldversion == GetCurrentVersion()) {
		return;
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
			NS_Node::NodeInterface* Node = Lib->GetInstance<NS_Node::NodeInterface>();
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