#include "Edge.hpp"
#include "../NodeInterface/NodeInterface.hpp"
#include "../Graph/Graph.hpp"


//#define DEBUG

Edge::Edge() {
	UID = 0;
	First = -1;
	Second = -1;
	FirstIO = -1;
	SecondIO = -1;
}

Edge::Edge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO) {
	UID = 0;
	this->First = First;
	this->Second = Second;
	this->FirstIO = FirstIO;
	this->SecondIO = SecondIO;
}

void Edge::SetUID(unsigned int UID) {
	this->UID = UID;
}

unsigned int Edge::GetUID() {
	return UID;
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

unsigned int Edge::GetFirst() {
	return First;
}

unsigned int Edge::GetSecond() {
	return Second;
}

unsigned int Edge::GetFirstIO() {
	return FirstIO;
}

unsigned int Edge::GetSecondIO() {
	return SecondIO;
}

nlohmann::json Edge::Serialize() {
	nlohmann::json data;
	data["UID"] = UID;
	data["First"] = First;
	data["Second"] = Second;
	data["FirstIO"] = FirstIO;
	data["SecondIO"] = SecondIO;
	return data;
}

void Edge::DeSerialize(nlohmann::json data, void* DCEE) {
	UID = data["UID"];
	First = data["First"];
	Second = data["Second"];
	FirstIO = data["FirstIO"];
	SecondIO = data["SecondIO"];
}