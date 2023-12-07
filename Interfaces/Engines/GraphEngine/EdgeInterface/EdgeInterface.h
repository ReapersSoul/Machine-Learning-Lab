#pragma once
#include "../../../SerializableInterface/SerializableInterface.h"
#include <string>
class GraphInterface;
class NodeInterface;

class EdgeInterface : public SerializableInterface
{
protected:
	unsigned int UID;
	std::string TypeID;
	unsigned int Priority;
	GraphInterface * Parent;
	unsigned int First;
	unsigned int Second;
	unsigned int FirstIO;
	unsigned int SecondIO;
public:
	EdgeInterface();

	EdgeInterface(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO);

	void Process(bool DirectionForward);

	void SetUID(unsigned int UID);

	unsigned int GetUID();

	std::string GetTypeID();

	void SetFirst(unsigned int First);

	void SetSecond(unsigned int Second);

	void SetFirstIO(unsigned int FirstIO);

	void SetSecondIO(unsigned int SecondIO);

	void SetParent(GraphInterface* Parent);

	GraphInterface* GetParent();

	NodeInterface* GetFirst();

	NodeInterface* GetSecond();

	nlohmann::json& GetFirstIO();

	nlohmann::json& GetSecondIO();

	nlohmann::json Serialize()override;

	void DeSerialize(nlohmann::json data, void* DCEE)override;
};