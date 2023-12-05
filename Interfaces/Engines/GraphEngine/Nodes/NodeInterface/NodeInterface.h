#pragma once
#include "../../SerializationEngine/SerializableInterface/SerializableInterface/SerializableInterface.h"

//imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

//forward declerations
class GraphInterface;
class NodeIOInterface;
class EdgeInterface;
class AttributeInterface;
class DynamicCodeExecutionEngineInterface;
class ActivationEngineInterface;
class LossEngineInterface;

class NodeInterface :public SerializableInterface {
protected:
	int x, y;
	unsigned int UID;
	std::string TypeID;
	unsigned int Priority;
	GraphInterface* ParentGraph;
	bool IsInput;
	bool IsOutput;
	bool Visited;
	nlohmann::json Description;
	std::vector<EdgeInterface*> InputEdges;
	std::vector<AttributeInterface*> Attributes;
	std::vector<EdgeInterface*> OutputEdges;
	std::vector<unsigned int> LockedInputs;
	std::vector<unsigned int> LockedOutputs;
	DynamicCodeExecutionEngineInterface* DCEE;
	ActivationEngineInterface* AE;
	LossEngineInterface* LE;
	bool UnlockAfterProcessing;
public:
	void PrintIOUID();

	bool HasInput(unsigned int UID);

	bool HasOutput(unsigned int UID);

	int InputCount();

	int OutputCount();

	void LockInput(unsigned int UID);

	void LockOutput(unsigned int UID);

	void UnlockInput(unsigned int UID);

	void UnlockOutput(unsigned int UID);

	bool InputIsLocked(unsigned int UID);

	bool OutputIsLocked(unsigned int UID);

	void ResetIOLocks();

	nlohmann::json GetInputDescriptors();

	nlohmann::json GetOutputDescriptors();

	void ChangeIOUID(unsigned int OldUID, unsigned int NewUID);

	void setXY(int x, int y);

	void setX(int x);

	void setY(int y);

	int getX();

	int getY();

	void ResetInputs();

	void ResetOutputs();

	void ResetIO();

	void SetDCEE(DynamicCodeExecutionEngineInterface* DCEE);

	void SetAE(ActivationEngineInterface* AE);

	void SetLE(LossEngineInterface* LE);

	void Visit();

	void Unvisit();

	bool IsVisited();

	void SetParentGraph(GraphInterface* ParentGraph);

	nlohmann::json FlattenData(nlohmann::json Data);

	nlohmann::json CreateMatrixData(std::vector<double> Data, int Rows, int Columns);

	nlohmann::json CreateMatrixData(std::vector<std::vector<double>> Data);

	nlohmann::json CreateTensorData(std::vector<double> Data, int Rows, int Columns, int Depth);

	nlohmann::json CreateTensorData(std::vector<std::vector<std::vector<double>>> Data);

	std::vector<std::vector<double>> GetMatrixFromData(nlohmann::json Data);

	std::vector<std::vector<std::vector<double>>> GetTensorFromData(nlohmann::json Data);

	std::vector<double> GetFirstVectorFromData(nlohmann::json Data);

	std::vector<std::vector<double>> GetFirstMatrixFromData(nlohmann::json Data);

	std::vector<std::vector<std::vector<double>>> GetFirstTensorFromData(nlohmann::json Data);

	GraphInterface* GetParentGraph();

	void SetUID(unsigned int UID);

	unsigned int GetUID();

	std::string GetTypeID();

	void SetTypeID(std::string TypeID);

	virtual nlohmann::json& GetDescription();

	void MakeInput(int line, std::string Name, std::string TypeID, nlohmann::json Data);
	void MakeOutput(int line, std::string Name, std::string TypeID, nlohmann::json Data);
	void MakeAttribute(int line, AttributeInterface* attribute);

	void RemoveAttribute(int line);
	void RemoveInput(int line);
	void RemoveOutput(int line);

	nlohmann::json& GetInputByIndex(int i);
	nlohmann::json& GetInputByUID(unsigned int UID);
	nlohmann::json& GetInputByName(std::string Name);

	std::vector<EdgeInterface*>& GetInputEdges();

	std::vector<AttributeInterface*>& GetAttributes();

	nlohmann::json& GetOutputByIndex(int i);
	nlohmann::json& GetOutputByUID(unsigned int UID);
	nlohmann::json& GetOutputByName(std::string Name);

	void ClearOutputData(int i);
	void ClearOutputData(std::string Name);
	void ClearInputData(int i);
	void ClearInputData(std::string Name);
	void ClearAllInputData();
	void ClearAllOutputData();

	void SetOutputByIndexData(int i, nlohmann::json Data);
	void SetOutputByUIDData(int i, nlohmann::json Data);
	void SetOutputByNameData(std::string Name, nlohmann::json Data);
	void SetInputByIndexData(int i, nlohmann::json Data);
	void SetInputByUIDData(int i, nlohmann::json Data);
	void SetInputByNameData(std::string Name, nlohmann::json Data);

	nlohmann::json& GetOutputDataByIndex(int i);
	nlohmann::json& GetOutputDataByUID(int i);
	nlohmann::json& GetOutputDataByName(std::string Name);
	nlohmann::json& GetInputDataByIndex(int i);
	nlohmann::json& GetInputDataByUID(int i);
	nlohmann::json& GetInputDataByName(std::string Name);

	std::vector<EdgeInterface*>& GetOutputEdges();

	virtual void Init() = 0;

	virtual void Process(bool Direction) = 0;

	virtual void Update() {};

	virtual void DrawNodeTitle(ImGuiContext* Context);

	virtual void DrawNodeProperties(ImGuiContext* Context);

	virtual NodeInterface* GetInstance() = 0;
	virtual NodeInterface* Clone() = 0;

	virtual nlohmann::json Serialize();
	virtual void DeSerialize(nlohmann::json data, void* DCEE);

	//virtual destructor	
	virtual ~NodeInterface() {};
};