#include "NodeInterface.hpp"
#include "../Edge/Edge.hpp"
#include "../Graph/Graph.hpp"
#include <stack>
namespace NS_Node
{
	void NodeInterface::ResetIO()
	{
		for (auto input : Inputs)
		{
			input->ClearData();
		}
		for (auto output : Outputs)
		{
			output->ClearData();
		}
	}

	bool NodeInterface::HasInput(unsigned int UID)
	{
		return std::find_if(Inputs.begin(), Inputs.end(), [UID](IO *IO)
							{ return IO->GetUID() == UID; }) != Inputs.end();
	}

	bool NodeInterface::HasOutput(unsigned int UID)
	{
		return std::find_if(Outputs.begin(), Outputs.end(), [UID](IO *IO)
							{ return IO->GetUID() == UID; }) != Outputs.end();
	}

	void NodeInterface::ChangeInputUID(unsigned int OldUID, unsigned int NewUID)
	{
		if (HasInput(OldUID))
		{
			auto it = std::find_if(Inputs.begin(), Inputs.end(), [OldUID](IO *IO)
								   { return IO->GetUID() == OldUID; });
			(*it)->SetUID(NewUID);
		}

		std::vector<Edge *>::iterator inputEdge = std::find_if(InputEdges.begin(), InputEdges.end(), [OldUID](Edge *Edge)
															   { return Edge->GetSecondIO() == OldUID; });
		if (inputEdge != InputEdges.end())
			(*inputEdge)->SetSecondIO(NewUID);
	}

	void NodeInterface::ChangeOutputUID(unsigned int OldUID, unsigned int NewUID)
	{
		if (HasOutput(OldUID))
		{
			auto it = std::find_if(Outputs.begin(), Outputs.end(), [OldUID](IO *IO)
								   { return IO->GetUID() == OldUID; });
			(*it)->SetUID(NewUID);
		}

		std::vector<Edge *>::iterator outputEdge = std::find_if(OutputEdges.begin(), OutputEdges.end(), [OldUID](Edge *Edge)
																{ return Edge->GetFirstIO() == OldUID; });
		if (outputEdge != OutputEdges.end())
			(*outputEdge)->SetFirstIO(NewUID);
	}

	void NodeInterface::setXY(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	void NodeInterface::setX(int x)
	{
		this->x = x;
	}

	void NodeInterface::setY(int y)
	{
		this->y = y;
	}

	int NodeInterface::getX()
	{
		return x;
	}

	int NodeInterface::getY()
	{
		return y;
	}

	void NodeInterface::SetDCEE(DynamicCodeExecutionEngineInterface *DCEE)
	{
		this->DCEE = DCEE;
	}

	void NodeInterface::SetAE(ActivationEngineInterface *AE)
	{
		this->AE = AE;
	}

	void NodeInterface::SetLE(LossEngineInterface *LE)
	{
		this->LE = LE;
	}

	void NodeInterface::Visit()
	{
		Visited = true;
	}

	void NodeInterface::Unvisit()
	{
		Visited = false;
	}

	bool NodeInterface::IsVisited()
	{
		return Visited;
	}

	void NodeInterface::SetParentGraph(Graph *ParentGraph)
	{
		this->ParentGraph = ParentGraph;
	}

	Graph *NodeInterface::GetParentGraph()
	{
		return ParentGraph;
	}

	void NodeInterface::SetUID(unsigned int UID)
	{
		this->UID = UID;
	}

	unsigned int NodeInterface::GetUID()
	{
		return UID;
	}

	std::string NodeInterface::GetTypeID()
	{
		return TypeID;
	}

	void NodeInterface::SetTypeID(std::string TypeID)
	{
		this->TypeID = TypeID;
	}

	unsigned int NodeInterface::MakeInput(unsigned int TypeID, std::function<void()> DrawFunction)
	{
		return ParentGraph->CreateInput(TypeID, DrawFunction);
	}

	unsigned int NodeInterface::MakeOutput(unsigned int TypeID, std::function<void()> DrawFunction)
	{
		return ParentGraph->CreateOutput(TypeID, DrawFunction);
	}

	unsigned int NodeInterface::MakeAttribute(Attribute *Attribute)
	{
		Attributes.push_back(Attribute);
		return Attributes.size() - 1;
	}

	void NodeInterface::MakeLine(unsigned int Input, unsigned int Attribute, unsigned int Output)
	{
		Line line;
		line.InputUID = Input;
		line.AttributeIndex = Attribute;
		line.OutputUID = Output;
		Lines.push_back(line);
	}

	int NodeInterface::GetLineCount()
	{
		return Lines.size();
	}

	std::vector<NodeInterface::Line> &NodeInterface::GetLines()
	{
		return Lines;
	}

	std::vector<Edge *> &NodeInterface::GetInputEdges()
	{
		return InputEdges;
	}

	std::vector<Attribute *> &NodeInterface::GetAttributes()
	{
		return Attributes;
	}

	IO *NodeInterface::GetInputByUID(unsigned int UID)
	{
		std::vector<IO *>::iterator it = std::find_if(Inputs.begin(), Inputs.end(), [UID](IO *IO)
													  { return IO->GetUID() == UID; });
		if (it != Inputs.end())
		{
			return (*it);
		}
		return nullptr;
	}

	IO *NodeInterface::GetInputByLine(unsigned int LineIndex)
	{
		return Lines[LineIndex].InputUID == -1 ? nullptr : Inputs[Lines[LineIndex].InputUID];
	}

	IO *NodeInterface::GetOutputByUID(unsigned int UID)
	{
		std::vector<IO *>::iterator it = std::find_if(Outputs.begin(), Outputs.end(), [UID](IO *IO)
													  { return IO->GetUID() == UID; });
		if (it != Outputs.end())
		{
			return (*it);
		}
		return nullptr;
	}

	IO *NodeInterface::GetOutputByLine(unsigned int LineIndex)
	{
		return Lines[LineIndex].OutputUID == -1 ? nullptr : Outputs[Lines[LineIndex].OutputUID];
	}

	std::vector<Edge *> &NodeInterface::GetOutputEdges()
	{
		return OutputEdges;
	}

	std::vector<IO *> &NodeInterface::GetInputs()
	{
		return Inputs;
	}

	std::vector<IO *> &NodeInterface::GetOutputs()
	{
		return Outputs;
	}

	void NodeInterface::DrawNodeTitle(ImGuiContext *Context)
	{
		ImGui::SetCurrentContext(Context);
		ImGui::Text(TypeID.c_str());
	}

	void NodeInterface::DrawNodeProperties(ImGuiContext *Context)
	{
		ImGui::SetCurrentContext(Context);
		ImGui::Text("Node Properties");
	}

	nlohmann::json NodeInterface::Serialize()
	{
		nlohmann::json data;
		data["x"] = getX();
		data["y"] = getY();
		data["TypeID"] = TypeID;
		data["UID"] = UID;

		data["InputEdges"] = nlohmann::json::array();
		for (auto inputEdge : InputEdges)
		{
			data["InputEdges"].push_back(inputEdge->GetUID());
		}

		data["OutputEdges"] = nlohmann::json::array();
		for (auto outputEdge : OutputEdges)
		{
			data["OutputEdges"].push_back(outputEdge->GetUID());
		}
		return data;
	}

	void NodeInterface::DeSerialize(nlohmann::json data, void *DCEE)
	{
		// set DCEE
		this->DCEE = (DynamicCodeExecutionEngineInterface *)DCEE;
		// set AE
		this->AE = ((DynamicCodeExecutionEngineInterface *)DCEE)->GetEngineInstanceAs<ActivationEngineInterface>("ActivationEngine");
		// set LE
		this->LE = ((DynamicCodeExecutionEngineInterface *)DCEE)->GetEngineInstanceAs<LossEngineInterface>("LossEngine");
		setXY(data["x"], data["y"]);
		UID = data["UID"];
		InputEdges.clear();
		OutputEdges.clear();
		Init();

		// set Description?

		for (auto inputEdge : data["InputEdges"])
		{
			InputEdges.push_back(&ParentGraph->GetEdges()[inputEdge]);
		}
		for (auto outputEdge : data["OutputEdges"])
		{
			OutputEdges.push_back(&ParentGraph->GetEdges()[outputEdge]);
		}
	}

	std::unordered_map<unsigned int, std::function<NodeInterface *()>> &Registrar::GetConstructors()
	{
		return Constructors;
	}

	std::unordered_map<std::string, unsigned int> &Registrar::GetTypeIDs()
	{
		return TypeIDs;
	}

	std::unordered_map<unsigned int, std::string> &Registrar::GetTypeIDsReverse()
	{
		return TypeIDsReverse;
	}

	Registrar::Registrar(){
		Constructors = {{0, []()
						 { return nullptr; }}};
		TypeIDs = {{"Invalid", 0}};
		TypeIDsReverse = {{0, "Invalid"}};
	}

	unsigned int Registrar::GetTypeID(std::string TypeID)
	{
		return TypeIDs[TypeID];
	}

	std::string Registrar::GetTypeID(unsigned int TypeID)
	{
		return TypeIDsReverse[TypeID];
	}

	bool Registrar::TypeIDExists(std::string TypeID)
	{
		return TypeIDs.find(TypeID) != TypeIDs.end();
	}

	unsigned int Registrar::RegisterType(std::string TypeID)
	{
		if (TypeIDs.find(TypeID) != TypeIDs.end())
		{
			return TypeIDs[TypeID];
		}
		unsigned int UID = std::hash<std::string>{}(TypeID);
		TypeIDs[TypeID] = UID;
		TypeIDsReverse[UID] = TypeID;
		return UID;
	}

	void Registrar::RegisterConstructor(unsigned int TypeID, std::function<NodeInterface *()> Constructor)
	{
		if (Constructors.find(TypeID) != Constructors.end())
			throw std::runtime_error("Constructor already registered");
		Constructors[TypeID] = Constructor;
	}

	void Registrar::RegisterNode(std::string TypeID, std::function<NodeInterface *()> Constructor)
	{
		if (!TypeIDExists(TypeID))
			RegisterConstructor(RegisterType(TypeID), Constructor);
		else
			RegisterConstructor(TypeIDs[TypeID], Constructor);
	}

	NodeInterface * Registrar::Construct(unsigned int TypeID)
	{
		return Constructors[TypeID]();
	}
}