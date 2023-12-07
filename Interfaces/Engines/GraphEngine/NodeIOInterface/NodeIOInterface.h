#pragma once
#include <vector>
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "../../DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.h"
#include "../../ActivationEngine/ActivationEngineInterface/ActivationEngineInterface.h"
#include "../../LossEngine/LossEngineInterface/LossEngineInterface.h"
#include "../../../SerializableInterface/SerializableInterface.h"

#include <nlohmann/json.hpp>

class GraphInterface;

//TODO: replace data with json data

class NodeIOInterface:public SerializableInterface {
protected:
	unsigned int UID;
	std::string TypeID;
	std::string Name;
	nlohmann::json Data;
	std::function<void(NodeIOInterface*)> DrawFunction;
	int size;
	GraphInterface* Parent;
	DynamicCodeExecutionEngineInterface* DCEE;
	ActivationEngineInterface* AE;
	LossEngineInterface* LE;
public:

	void SetParent(GraphInterface* Parent);

	void SetDCEE(DynamicCodeExecutionEngineInterface* DCEE);

	void SetAE(ActivationEngineInterface* AE);

	void SetLE(LossEngineInterface* LE);

	nlohmann::json& GetData();

	std::string GetTypeID();

	void SetTypeID(std::string TypeID);

	void SetSize(int size);

	int GetSize();

	void SetDrawFunction(std::function<void(NodeIOInterface*)> DrawFunction);

	std::string GetName();

	void SetName(std::string Name);

	void SetUID(unsigned int UID);

	unsigned int GetUID();

	NodeIOInterface();

	~NodeIOInterface();

	virtual void DrawIO(ImGuiContext* Context);

	nlohmann::json Serialize() override;

	void DeSerialize(nlohmann::json data, void* DCEE) override;
};