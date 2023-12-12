#pragma once
#include "../../../SerializableInterface/SerializableInterface.hpp"
#include <string>

class Edge : public SerializableInterface
{
protected:
	unsigned int UID;
	unsigned int First;
	unsigned int Second;
	unsigned int FirstIO;
	unsigned int SecondIO;
public:
	Edge();

	Edge(unsigned int First, unsigned int Second, unsigned int FirstIO, unsigned int SecondIO);

	void SetUID(unsigned int UID);

	unsigned int GetUID();

	unsigned int GetTypeID();

	void SetFirst(unsigned int First);

	void SetSecond(unsigned int Second);

	void SetFirstIO(unsigned int FirstIO);

	void SetSecondIO(unsigned int SecondIO);

	unsigned int GetFirst();

	unsigned int GetSecond();

	unsigned int GetFirstIO();

	unsigned int GetSecondIO();

	nlohmann::json Serialize()override;

	void DeSerialize(nlohmann::json data, void* DCEE)override;
};