#include "NodeInterface.hpp"
#include "../Edge/Edge.hpp"
#include "../Graph/Graph.hpp"
#include <stack>

namespace NodeInterface {

	bool NodeInterface::HasInput(unsigned int UID)
	{
		return std::find_if(Inputs.begin(), Inputs.end(), [UID](Input IO) {return IO.GetUID() == UID; }) != Inputs.end();
	}

	void NodeInterface::PrintIOUID() {

	}

	bool NodeInterface::HasOutput(unsigned int UID)
	{
		return std::find_if(Outputs.begin(), Outputs.end(), [UID](Output IO) {return IO.GetUID() == UID; }) != Outputs.end();
	}

	void NodeInterface::ChangeIOUID(unsigned int OldUID, unsigned int NewUID)
	{
		if (HasInput(OldUID))
			Inputs[std::find_if(Inputs.begin(), Inputs.end(), [OldUID](Input IO) {return IO.GetUID() == OldUID; }) - Inputs.begin()].SetUID(NewUID);
		if (HasOutput(OldUID))
			Outputs[std::find_if(Outputs.begin(), Outputs.end(), [OldUID](Output IO) {return IO.GetUID() == OldUID; }) - Outputs.begin()].SetUID(NewUID);


		std::vector<EdgeInterface*>::iterator inputEdge = std::find_if(InputEdges.begin(), InputEdges.end(), [OldUID](EdgeInterface* Edge) {return Edge->GetSecondIO()->GetUID() == OldUID; });
		if (inputEdge != InputEdges.end())
			(*inputEdge)->SetSecondIO(NewUID);

		std::vector<EdgeInterface*>::iterator outputEdge = std::find_if(OutputEdges.begin(), OutputEdges.end(), [OldUID](EdgeInterface* Edge) {return Edge->GetFirstIO()->GetUID() == OldUID; });
		if (outputEdge != OutputEdges.end())
			(*outputEdge)->SetFirstIO(NewUID);
	}

	void NodeInterface::setXY(int x, int y) {
		this->x = x;
		this->y = y;
	}

	void NodeInterface::setX(int x) {
		this->x = x;
	}

	void NodeInterface::setY(int y) {
		this->y = y;
	}

	int  NodeInterface::getX() {
		return x;
	}

	int  NodeInterface::getY() {
		return y;
	}

	void NodeInterface::SetDCEE(DynamicCodeExecutionEngineInterface* DCEE) {
		this->DCEE = DCEE;
	}

	void NodeInterface::SetAE(ActivationEngineInterface* AE) {
		this->AE = AE;
	}

	void NodeInterface::SetLE(LossEngineInterface* LE) {
		this->LE = LE;
	}

	void NodeInterface::Visit() {
		Visited = true;
	}

	void NodeInterface::Unvisit() {
		Visited = false;
	}

	bool  NodeInterface::IsVisited() {
		return Visited;
	}

	void NodeInterface::SetParentGraph(Graph* ParentGraph) {
		this->ParentGraph = ParentGraph;
	}

	Graph* NodeInterface::GetParentGraph() {
		return ParentGraph;
	}

	void NodeInterface::SetUID(unsigned int UID) {
		this->UID = UID;
	}

	unsigned int  NodeInterface::GetUID() {
		return UID;
	}

	std::string  NodeInterface::GetTypeID() {
		return TypeID;
	}

	void NodeInterface::SetTypeID(std::string TypeID) {
		this->TypeID = TypeID;
	}

	void NodeInterface::MakeLine(Input Input, Attribute* Attribute, Output Output)
	{
		Line line;
		if (Input.GetUID() != -1) {
			Inputs.push_back(Input);
			line.InputUID = Input.GetUID();
		}
		if (Attribute != nullptr) {
			Attributes.push_back(Attribute);
			line.AttributeIndex = Attributes.size() - 1;
		}
		if (Output.GetUID() != -1) {
			Outputs.push_back(Output);
			line.OutputUID = Output.GetUID();
		}
		Lines.push_back(line);
	}

	std::vector<EdgeInterface*>& NodeInterface::GetInputEdges() {
		return InputEdges;
	}

	std::vector<Attribute*>& NodeInterface::GetAttributes() {
		return Attributes;
	}

	Input* NodeInterface::GetInputByUID(unsigned int UID)
	{
		std::vector<Input>::iterator it = std::find_if(Inputs.begin(), Inputs.end(), [UID](Input IO) {return IO.GetUID() == UID; });
		if (it != Inputs.end()) {
			return &(*it);
		}
		return nullptr;
	}

	Output* NodeInterface::GetOutputByUID(unsigned int UID)
	{
		std::vector<Output>::iterator it = std::find_if(Outputs.begin(), Outputs.end(), [UID](Output IO) {return IO.GetUID() == UID; });
		if (it != Outputs.end()) {
			return &(*it);
		}
		return nullptr;
	}

	std::vector<EdgeInterface*>& NodeInterface::GetOutputEdges() {
		return OutputEdges;
	}

	std::vector<Input>& NodeInterface::GetInputs()
	{
		return Inputs;
	}

	std::vector<Output>& NodeInterface::GetOutputs()
	{
		return Outputs;
	}

	void NodeInterface::DrawNodeTitle(ImGuiContext* Context) {
		ImGui::SetCurrentContext(Context);
		ImGui::Text(TypeID.c_str());
	}

	void NodeInterface::DrawNodeProperties(ImGuiContext* Context)
	{
		ImGui::SetCurrentContext(Context);
		ImGui::Text("Node Properties");
	}

	nlohmann::json NodeInterface::Serialize() {
		nlohmann::json data;
		data["x"] = getX();
		data["y"] = getY();
		data["TypeID"] = TypeID;
		data["UID"] = UID;

		data["InputEdges"] = nlohmann::json::array();
		for (auto inputEdge : InputEdges) {
			data["InputEdges"].push_back(inputEdge->GetUID());
		}

		data["OutputEdges"] = nlohmann::json::array();
		for (auto outputEdge : OutputEdges) {
			data["OutputEdges"].push_back(outputEdge->GetUID());
		}
		return data;
	}

	void NodeInterface::DeSerialize(nlohmann::json data, void* DCEE) {
		//set DCEE
		this->DCEE = (DynamicCodeExecutionEngineInterface*)DCEE;
		//set AE
		this->AE = ((DynamicCodeExecutionEngineInterface*)DCEE)->GetEngineInstanceAs<ActivationEngineInterface>("ActivationEngine");
		//set LE
		this->LE = ((DynamicCodeExecutionEngineInterface*)DCEE)->GetEngineInstanceAs<LossEngineInterface>("LossEngine");
		setXY(data["x"], data["y"]);
		UID = data["UID"];
		InputEdges.clear();
		OutputEdges.clear();
		Init();

		//set Description?

		for (auto inputEdge : data["InputEdges"]) {
			InputEdges.push_back(ParentGraph->GetEdges()[inputEdge]);
		}
		for (auto outputEdge : data["OutputEdges"]) {
			OutputEdges.push_back(ParentGraph->GetEdges()[outputEdge]);
		}
	}
}