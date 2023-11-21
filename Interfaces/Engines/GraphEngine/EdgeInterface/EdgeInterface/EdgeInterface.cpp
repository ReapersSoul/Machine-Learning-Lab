#include "EdgeInterface.h"
#include "../../GraphInterface/GraphInterface/GraphInterface.h"

//#define DEBUG

EdgeInterface::EdgeInterface() {
	UID = 0;
	TypeID = "Edge";
	Priority = 0;
	First = -1;
	Second = -1;
	FirstIO = -1;
	SecondIO = -1;
	Parent = nullptr;
}

EdgeInterface::EdgeInterface(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO) {
	UID = 0;
	TypeID = "Edge";
	Priority = 0;
	this->First = First;
	this->Second = Second;
	this->FirstIO = FirstIO;
	this->SecondIO = SecondIO;
	Parent = nullptr;
}

void EdgeInterface::Process(bool DirectionForward) {
	if (DirectionForward) {
		//copy data from first to second
		try {
#ifdef DEBUG
			printf("Transfering data from %s %d IO %d to %s %d IO %d:\n%s\n\n\n\n", Parent->GetNodes()[First]->GetTypeID().c_str(), First, FirstIO, Parent->GetNodes()[Second]->GetTypeID().c_str(), Second, SecondIO, Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO).dump(4).c_str());
#else
			//printf("Transfering data from %s %d IO %d to %s %d IO %d\n\n\n\n", Parent->GetNodes()[First]->GetTypeID().c_str(), First, FirstIO, Parent->GetNodes()[Second]->GetTypeID().c_str(), Second, SecondIO);
#endif // DEBUG
			if(!Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO).is_array())
			Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO) = nlohmann::json::array();
			if(!Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO).is_null()&&!Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO).empty())
			Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO).push_back(Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO));
#ifdef DEBUG
			printf("Data transfered\n%s %d IO %d Data now equals:\n%s\n\n\n\n", Parent->GetNodes()[Second]->GetTypeID().c_str(), Second, SecondIO, Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO).dump(4).c_str());
#endif // DEBUG
		}
		catch (nlohmann::json::parse_error pe) {
			printf("Error: %s\n", pe.what());
		}
		catch (nlohmann::json::out_of_range oor) {
			printf("Error: %s\n", oor.what());
		}
		catch (nlohmann::json::other_error oe) {
			printf("Error: %s\n", oe.what());
		}
		catch (nlohmann::json::type_error te) {
			printf("Error: %s\n", te.what());
		}
	}
	else {
		try {
			//copy data from second to first
#ifdef DEBUG
			printf("Transfering data from %s %d IO %d to %s %d IO %d:\n%s\n\n\n\n",Parent->GetNodes()[Second]->GetTypeID().c_str(), Second, SecondIO, Parent->GetNodes()[First]->GetTypeID().c_str(), First, FirstIO, Parent->GetNodes()[Second]->GetInputByUID(SecondIO).dump(4).c_str());
#else
			//printf("Transfering data from %s %d IO %d to %s %d IO %d\n\n\n\n", Parent->GetNodes()[Second]->GetTypeID().c_str(), Second, SecondIO, Parent->GetNodes()[First]->GetTypeID().c_str(), First, FirstIO);
#endif // DEBUG
			if (!Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO).is_array())
			Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO) = nlohmann::json::array();
			if (!Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO).is_null() && !Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO).empty())
			Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO).push_back(Parent->GetNodes()[Second]->GetInputDataByUID(SecondIO));
#ifdef DEBUG
			printf("Data transfered\n%s %d IO %d Data now equals:\n%s\n\n\n\n", Parent->GetNodes()[First]->GetTypeID().c_str(), First, FirstIO, Parent->GetNodes()[First]->GetOutputDataByUID(FirstIO).dump(4).c_str());
#endif // DEBUG
		}
		catch (nlohmann::json::parse_error pe) {
			printf("Error: %s\n", pe.what());
		}
		catch (nlohmann::json::out_of_range oor) {
			printf("Error: %s\n", oor.what());
		}
		catch (nlohmann::json::other_error oe) {
			printf("Error: %s\n", oe.what());
		}
		catch (nlohmann::json::type_error te) {
			printf("Error: %s\n", te.what());
		}
	}
}

void EdgeInterface::SetUID(unsigned int UID) {
	this->UID = UID;
}

unsigned int EdgeInterface::GetUID() {
	return UID;
}

std::string EdgeInterface::GetTypeID() {
	return TypeID;
}

void EdgeInterface::SetFirst(unsigned int First) {
	this->First = First;
}

void EdgeInterface::SetSecond(unsigned int Second) {
	this->Second = Second;
}

void EdgeInterface::SetFirstIO(unsigned int FirstIO) {
	this->FirstIO = FirstIO;
}

void EdgeInterface::SetSecondIO(unsigned int SecondIO) {
	this->SecondIO = SecondIO;
}

void EdgeInterface::SetParent(GraphInterface* Parent) {
	this->Parent = Parent;
}

GraphInterface* EdgeInterface::GetParent() {
	return Parent;
}

NodeInterface* EdgeInterface::GetFirst() {
	return Parent->GetNodes()[First];
}

NodeInterface* EdgeInterface::GetSecond() {
	return Parent->GetNodes()[Second];
}

nlohmann::json& EdgeInterface::GetFirstIO() {
	return Parent->GetNodes()[First]->GetOutputByUID(FirstIO);
}

nlohmann::json& EdgeInterface::GetSecondIO() {
	return Parent->GetNodes()[Second]->GetInputByUID(SecondIO);
}

nlohmann::json EdgeInterface::Serialize() {
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

void EdgeInterface::DeSerialize(nlohmann::json data, void* DCEE) {
	UID = data["UID"];
	TypeID = data["TypeID"];
	Priority = data["Priority"];
	First = data["First"];
	Second = data["Second"];
	FirstIO = data["FirstIO"];
	SecondIO = data["SecondIO"];
}