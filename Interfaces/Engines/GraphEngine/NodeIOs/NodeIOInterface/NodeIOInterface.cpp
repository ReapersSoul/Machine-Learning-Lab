#include "NodeIOInterface.h"
#include "../../GraphInterface/GraphInterface/GraphInterface.h"

void NodeIOInterface::SetParent(GraphInterface* Parent)
{
	this->Parent = Parent;
}

void NodeIOInterface::SetDCEE(DynamicCodeExecutionEngineInterface* DCEE) {
	this->DCEE = DCEE;
}

void NodeIOInterface::SetAE(ActivationEngineInterface* AE) {
	this->AE = AE;
}

void NodeIOInterface::SetLE(LossEngineInterface* LE) {
	this->LE = LE;
}

nlohmann::json& NodeIOInterface::GetData() {
	return Data;
}

std::string NodeIOInterface::GetTypeID() {
	return TypeID;
}

void NodeIOInterface::SetTypeID(std::string TypeID) {
	this->TypeID = TypeID;
}

void NodeIOInterface::SetSize(int size)
{
	this->size = size;
}

int NodeIOInterface::GetSize()
{
	return size;
}

void NodeIOInterface::SetDrawFunction(std::function<void(NodeIOInterface*)> DrawFunction)
{
	this->DrawFunction = DrawFunction;
}

std::string NodeIOInterface::GetName() {
	return Name;
}

void NodeIOInterface::SetName(std::string Name) {
	this->Name = Name;
}

void NodeIOInterface::SetUID(unsigned int UID) {
	this->UID = UID;
}

unsigned int NodeIOInterface::GetUID() {
	return UID;
}

NodeIOInterface::NodeIOInterface() {
}

NodeIOInterface::~NodeIOInterface()
{
}

void NodeIOInterface::DrawIO(ImGuiContext* Context) {
	ImGui::SetCurrentContext(Context);
	if (DrawFunction) {
		DrawFunction(this);
	}
	else {
		ImGui::Text(Name.c_str());
	}
}

nlohmann::json NodeIOInterface::Serialize(){
	nlohmann::json data;
	//uid
	data["UID"] = UID;
	//typeid
	data["TypeID"] = TypeID;
	//name
	data["Name"] = Name;
	//size of datatype
	data["Size"] = size;
	//data
	data["Data"] = Data;
	//graph name
	data["GraphName"] = Parent->GetName();
	return data;
}

void NodeIOInterface::DeSerialize(nlohmann::json data, void* DCEE) {
	//uid
	UID = data["UID"];
	//typeid
	TypeID = data["TypeID"];
	//name
	Name = data["Name"];
	//size of datatype
	size = data["Size"];
	//data
	Data.push_back(data["Data"]);
}