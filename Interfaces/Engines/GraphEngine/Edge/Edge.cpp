#include "Edge.hpp"
#include "../Graph/Graph.hpp"

//#define DEBUG

namespace NodeInterface {
	class NodeInterface;
}

Edge::Edge() {
	UID = 0;
	TypeID = "Edge";
	Priority = 0;
	First = -1;
	Second = -1;
	FirstIO = -1;
	SecondIO = -1;
	Parent = nullptr;
}

Edge::Edge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO) {
	UID = 0;
	TypeID = "Edge";
	Priority = 0;
	this->First = First;
	this->Second = Second;
	this->FirstIO = FirstIO;
	this->SecondIO = SecondIO;
	Parent = nullptr;
}

void Edge::SetUID(unsigned int UID) {
	this->UID = UID;
}

unsigned int Edge::GetUID() {
	return UID;
}

std::string Edge::GetTypeID() {
	return TypeID;
}

void Edge::SetFirst(unsigned int First) {
	this->First = First;
}

void Edge::SetSecond(unsigned int Second) {
	this->Second = Second;
}

void Edge::SetFirstIO(unsigned int FirstIO) {
	this->FirstIO = FirstIO;
}

void Edge::SetSecondIO(unsigned int SecondIO) {
	this->SecondIO = SecondIO;
}

void Edge::SetParent(Graph* Parent) {
	this->Parent = Parent;
}

Graph* Edge::GetParent() {
	return Parent;
}

NodeInterface::NodeInterface* Edge::GetFirst() {
	return Parent->GetNodes()[First];
}

NodeInterface::NodeInterface* Edge::GetSecond() {
	return Parent->GetNodes()[Second];
}

Output* Edge::GetFirstIO() {
	return Parent->GetNodes()[First]->GetOutputByUID(FirstIO);
}

Input* Edge::GetSecondIO() {
	return Parent->GetNodes()[Second]->GetInputByUID(SecondIO);
}

nlohmann::json Edge::Serialize() {
	nlohmann::json data;
	data["UID"] = UID;
	data["TypeID"] = TypeID;
	data["Priority"] = Priority;
	data["First"] = First;
	data["Second"] = Second;
	data["FirstIO"] = FirstIO;
	data["SecondIO"] = SecondIO;
	return data;
}

void Edge::DeSerialize(nlohmann::json data, void* DCEE) {
	UID = data["UID"];
	TypeID = data["TypeID"];
	Priority = data["Priority"];
	First = data["First"];
	Second = data["Second"];
	FirstIO = data["FirstIO"];
	SecondIO = data["SecondIO"];
}