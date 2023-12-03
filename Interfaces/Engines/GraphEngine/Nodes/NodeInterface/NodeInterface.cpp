#include "NodeInterface.h"
#include "../../NodeIOs/NodeIOInterface/NodeIOInterface.h"
#include "../EdgeInterface/EdgeInterface.h"
#include "../GraphInterface/GraphInterface.h"
#include <stack>

bool NodeInterface::HasInput(unsigned int UID)
{
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if(Description[i]["Input"]["UID"]==UID)
				return true;
		}
	}
	return false;
}

void NodeInterface::PrintIOUID() {
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if (Description[i]["Input"].find("UID") != Description[i]["Input"].end()) {
				printf("Input UID: %d\n", Description[i]["Input"]["UID"].get<unsigned int>());
			}
		}
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"].find("UID") != Description[i]["Output"].end()) {
				printf("Output UID: %d\n", Description[i]["Output"]["UID"].get<unsigned int>());
			}
		}
	}
}

bool NodeInterface::HasOutput(unsigned int UID)
{
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"]["UID"] == UID)
				return true;
		}
	}
	return false;
}

void NodeInterface::ChangeIOUID(unsigned int OldUID, unsigned int NewUID)
{
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if (Description[i]["Input"]["UID"] == OldUID)
				Description[i]["Input"]["UID"] = NewUID;
		}
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"]["UID"] == OldUID)
				Description[i]["Output"]["UID"] = NewUID;
		}
	}

	for (auto& Edge : InputEdges) {
		if (Edge->GetSecondIO()["UID"] == OldUID)
			Edge->SetSecondIO(NewUID);
	}
	for (auto& Edge : OutputEdges) {
		if (Edge->GetFirstIO()["UID"] == OldUID)
			Edge->SetFirstIO(NewUID);
	}
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

nlohmann::json NodeInterface::FlattenData(nlohmann::json Data)
{
	nlohmann::json FlattenedData;

	std::stack<nlohmann::json> stack;
	stack.push(Data);

	while (!stack.empty()) {
		nlohmann::json current = stack.top();
		stack.pop();

		if (current.is_array()) {
			for (int i = current.size() - 1; i >= 0; i--) {
				stack.push(current[i]);
			}
		}
		else if (current.is_object()) {
			for (auto it = current.begin(); it != current.end(); ++it) {
				stack.push(it.value());
			}
		}
		else {
			FlattenedData.push_back(current);
		}
	}

	return FlattenedData;
}

nlohmann::json NodeInterface::CreateMatrixData(std::vector<double> Data, int Rows, int Columns) {
	nlohmann::json MatrixData;
	MatrixData["Rows"] = Rows;
	MatrixData["Columns"] = Columns;
	MatrixData["Data"] = Data;
	return MatrixData;
}

nlohmann::json NodeInterface::CreateMatrixData(std::vector<std::vector<double>> Data) {
	nlohmann::json MatrixData;
	MatrixData["Rows"] = Data.size();
	MatrixData["Columns"] = Data[0].size();
	MatrixData["Data"] = Data;
	return MatrixData;
}

nlohmann::json NodeInterface::CreateTensorData(std::vector<double> Data, int Rows, int Columns, int Depth) {
	nlohmann::json TensorData;
	TensorData["Rows"] = Rows;
	TensorData["Columns"] = Columns;
	TensorData["Depth"] = Depth;
	TensorData["Data"] = Data;
	return TensorData;
}

nlohmann::json NodeInterface::CreateTensorData(std::vector<std::vector<std::vector<double>>> Data) {
	nlohmann::json TensorData;
	TensorData["Rows"] = Data.size();
	TensorData["Columns"] = Data[0].size();
	TensorData["Depth"] = Data[0][0].size();
	TensorData["Data"] = Data;
	return TensorData;
}

std::vector<std::vector<double>> NodeInterface::GetMatrixFromData(nlohmann::json Data) {
	std::vector<std::vector<double>> Matrix;
	for (int i = 0; i < Data["Rows"]; i++)
	{
		std::vector<double> Row;
		for (int j = 0; j < Data["Columns"]; j++)
		{
			Row.push_back(Data["Data"][i * Data["Columns"] + j]);
		}
		Matrix.push_back(Row);
	}
	return Matrix;
}

std::vector<std::vector<std::vector<double>>> NodeInterface::GetTensorFromData(nlohmann::json Data) {
	std::vector<std::vector<std::vector<double>>> Tensor;
	for (int i = 0; i < Data["Rows"]; i++)
	{
		std::vector<std::vector<double>> Matrix;
		for (int j = 0; j < Data["Columns"]; j++)
		{
			std::vector<double> Row;
			for (int k = 0; k < Data["Depth"]; k++)
			{
				Row.push_back(Data["Data"][i * Data["Columns"] * Data["Depth"] + j * Data["Depth"] + k]);
			}
			Matrix.push_back(Row);
		}
		Tensor.push_back(Matrix);
	}
	return Tensor;
}

//TODO: Impliment properly
std::vector<double> NodeInterface::GetFirstVectorFromData(nlohmann::json Data) {
	std::vector<double> Vector;
	//search for first vector of numbers
	for (int i = 0; i < Data.size(); i++)
	{
		if (!Data[i].is_number()) {
			Vector=GetFirstVectorFromData(Data[i]);
			if(Vector.size()>0)
				return Vector;
		}
		else {
			Vector.push_back(Data[i].get<double>());
		}
	}
	return Vector;
}

//TODO: Impliment properly
std::vector<std::vector<double>> NodeInterface::GetFirstMatrixFromData(nlohmann::json Data) {
	std::vector<std::vector<double>> Matrix;

	return Matrix;
}

//TODO: Impliment properly
std::vector<std::vector<std::vector<double>>> NodeInterface::GetFirstTensorFromData(nlohmann::json Data) {
	std::vector<std::vector<std::vector<double>>> Tensor;

	return Tensor;
}

void NodeInterface::ResetInputs() {
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			Description[i]["Input"]["Data"]=nlohmann::json();
		}
	}
}

void NodeInterface::ResetOutputs() {
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			Description[i]["Output"]["Data"] = nlohmann::json();
		}
	}
}

void NodeInterface::ResetIO() {
	ResetInputs();
	ResetOutputs();
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

void NodeInterface::SetParentGraph(GraphInterface* ParentGraph) {
	this->ParentGraph = ParentGraph;
}

GraphInterface* NodeInterface::GetParentGraph() {
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

nlohmann::json& NodeInterface::GetDescription()
{
	return Description;
}

void NodeInterface::MakeInput(int line, std::string Name, std::string TypeID, nlohmann::json Data) {
	unsigned int IOUID = ParentGraph->GetNextIOUID(UID);
	Description[line]["Input"]["UID"] = IOUID;
	Description[line]["Input"]["Name"] = Name;
	Description[line]["Input"]["TypeID"] = TypeID;
	Description[line]["Input"]["Data"] = Data;
}
void NodeInterface::MakeOutput(int line, std::string Name, std::string TypeID, nlohmann::json Data) {
	unsigned int IOUID = ParentGraph->GetNextIOUID(UID);
	Description[line]["Output"]["UID"] = IOUID;
	Description[line]["Output"]["Name"] = Name;
	Description[line]["Output"]["TypeID"] = TypeID;
	Description[line]["Output"]["Data"] = Data;
}
void NodeInterface::MakeAttribute(int line, AttributeInterface* attribute) {
	Attributes.push_back(attribute);
	Description[line]["Attribute"] = Attributes.size() - 1;
}

void NodeInterface::RemoveAttribute(int line) {
	Attributes.erase(Attributes.begin() + Description[line]["Attribute"].get<int>());
	Description[line].erase("Attribute");
}
void NodeInterface::RemoveInput(int line) {
	Description[line].erase("Input");
}
void NodeInterface::RemoveOutput(int line) {
	Description[line].erase("Output");
}

nlohmann::json& NodeInterface::GetInputByIndex(int i) {
	return Description[i]["Input"];
}

nlohmann::json& NodeInterface::GetInputByUID(unsigned int i) {
	for (int j = 0; j < Description.size(); j++)
	{
		if (Description[j].find("Input") != Description[j].end()) {
			if (Description[j]["Input"]["UID"].get<unsigned int>() == i) {
				return Description[j]["Input"];
			}
		}
	}
	//return empty json if not found
	return Description.end().value();
}

nlohmann::json& NodeInterface::GetInputByName(std::string name) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if (Description[i]["Input"]["Name"].get<std::string>() == name) {
				return Description[i]["Input"];
			}
		}
	}
	//return empty json if not found
	return Description.end().value();
}

void NodeInterface::ClearOutputData(int i) {
	Description[i]["Output"]["Data"].clear();
}
void NodeInterface::ClearOutputData(std::string Name) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"]["Name"].get<std::string>() == Name) {
				Description[i]["Output"]["Data"].clear();
			}
		}
	}
}
void NodeInterface::ClearInputData(int i) {
	Description[i]["Input"]["Data"].clear();
}
void NodeInterface::ClearInputData(std::string Name) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if (Description[i]["Input"]["Name"].get<std::string>() == Name) {
				Description[i]["Input"]["Data"].clear();
			}
		}
	}
}
void NodeInterface::ClearAllInputData() {
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			Description[i]["Input"]["Data"].clear();
		}
	}
}
void NodeInterface::ClearAllOutputData() {
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			Description[i]["Output"]["Data"].clear();
		}
	}
}

void NodeInterface::SetOutputByIndexData(int i, nlohmann::json Data) {
	Description[i]["Output"]["Data"] = Data;
}
void NodeInterface::SetOutputByUIDData(int i, nlohmann::json Data) {
	for (int j = 0; j < Description.size(); j++)
	{
		if (Description[j].find("Output") != Description[j].end()) {
			if (Description[j]["Output"]["UID"].get<unsigned int>() == i) {
				Description[j]["Output"]["Data"] = Data;
			}
		}
	}
}
void NodeInterface::SetOutputByNameData(std::string Name, nlohmann::json Data) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"]["Name"].get<std::string>() == Name) {
				Description[i]["Output"]["Data"] = Data;
			}
		}
	}
}
void NodeInterface::SetInputByIndexData(int i, nlohmann::json Data) {
	Description[i]["Input"]["Data"] = Data;
}
void NodeInterface::SetInputByUIDData(int i, nlohmann::json Data) {
	for (int j = 0; j < Description.size(); j++)
	{
		if (Description[j].find("Input") != Description[j].end()) {
			if (Description[j]["Input"]["UID"].get<unsigned int>() == i) {
				Description[j]["Input"]["Data"] = Data;
			}
		}
	}
}
void NodeInterface::SetInputByNameData(std::string Name, nlohmann::json Data) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if (Description[i]["Input"]["Name"].get<std::string>() == Name) {
				Description[i]["Input"]["Data"] = Data;
			}
		}
	}
}

nlohmann::json& NodeInterface::GetOutputDataByIndex(int i) {
	return Description[i]["Output"]["Data"];
}
nlohmann::json& NodeInterface::GetOutputDataByUID(int i) {
	for (int j = 0; j < Description.size(); j++)
	{
		if (Description[j].find("Output") != Description[j].end()) {
			if (Description[j]["Output"]["UID"].get<unsigned int>() == i) {
				return Description[j]["Output"]["Data"];
			}
		}
	}
}
nlohmann::json& NodeInterface::GetOutputDataByName(std::string Name) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"]["Name"].get<std::string>() == Name) {
				return Description[i]["Output"]["Data"];
			}
		}
	}
}
nlohmann::json& NodeInterface::GetInputDataByIndex(int i) {
	return Description[i]["Input"]["Data"];
}
nlohmann::json& NodeInterface::GetInputDataByUID(int i) {
	for (int j = 0; j < Description.size(); j++)
	{
		if (Description[j].find("Input") != Description[j].end()) {
			if (Description[j]["Input"]["UID"].get<unsigned int>() == i) {
				return Description[j]["Input"]["Data"];
			}
		}
	}
}
nlohmann::json& NodeInterface::GetInputDataByName(std::string Name) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Input") != Description[i].end()) {
			if (Description[i]["Input"]["Name"].get<std::string>() == Name) {
				return Description[i]["Input"]["Data"];
			}
		}
	}
}

std::vector<EdgeInterface*>& NodeInterface::GetInputEdges() {
	return InputEdges;
}

std::vector<AttributeInterface*>& NodeInterface::GetAttributes() {
	return Attributes;
}

nlohmann::json& NodeInterface::GetOutputByIndex(int i) {
	return Description[i]["Output"];
}

nlohmann::json& NodeInterface::GetOutputByUID(unsigned int i) {
	for (int j = 0; j < Description.size(); j++)
	{
		if (Description[j].find("Output") != Description[j].end()) {
			if (Description[j]["Output"]["UID"].get<unsigned int>() == i) {
				return Description[j]["Output"];
			}
		}
	}
}

nlohmann::json& NodeInterface::GetOutputByName(std::string name) {
	//find the line with the name
	for (int i = 0; i < Description.size(); i++)
	{
		if (Description[i].find("Output") != Description[i].end()) {
			if (Description[i]["Output"]["Name"].get<std::string>() == name) {
				return Description[i]["Output"];
			}
		}
	}
}

std::vector<EdgeInterface*>& NodeInterface::GetOutputEdges() {
	return OutputEdges;
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

nlohmann::json NodeInterface::Serialize(){
	nlohmann::json data;
	data["x"] = getX();
	data["y"] = getY();
	data["TypeID"] = TypeID;
	data["UID"] = UID;

	data["Description"] = Description;

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