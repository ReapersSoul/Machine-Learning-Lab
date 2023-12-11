#pragma once
#include "../../../SerializableInterface/SerializableInterface.hpp"
#include "../Input/Input.hpp"
#include "../Output/Output.hpp"
#include <string>
class Graph;
namespace NodeInterface {
	class NodeInterface;
}

class Edge : public SerializableInterface
{
protected:
	unsigned int UID;
	std::string TypeID;
	unsigned int Priority;
	Graph * Parent;
	unsigned int First;
	unsigned int Second;
	unsigned int FirstIO;
	unsigned int SecondIO;
public:
	Edge();

	Edge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO);

	void SetUID(unsigned int UID);

	unsigned int GetUID();

	std::string GetTypeID();

	void SetFirst(unsigned int First);

	void SetSecond(unsigned int Second);

	void SetFirstIO(unsigned int FirstIO);

	void SetSecondIO(unsigned int SecondIO);

	void SetParent(Graph* Parent);

	Graph* GetParent();

	NodeInterface::NodeInterface* GetFirst();

	NodeInterface::NodeInterface* GetSecond();

	Output* GetFirstIO();

	Input* GetSecondIO();

	nlohmann::json Serialize()override;

	void DeSerialize(nlohmann::json data, void* DCEE)override;
};