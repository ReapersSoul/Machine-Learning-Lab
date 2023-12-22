#pragma once
#include "../../../SerializableInterface/SerializableInterface.hpp"
#include "../IO/IO.hpp"

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// forward declerations
class Graph;
class Edge;
class Attribute;
class DynamicCodeExecutionEngineInterface;
class ActivationEngineInterface;
class LossEngineInterface;

namespace NS_Node
{
	class NodeInterface : public SerializableInterface
	{
	protected:
		int x, y;
		unsigned int UID;
		std::string TypeID;
		unsigned int Priority;
		Graph *ParentGraph;
		bool IsInput;
		bool IsOutput;
		bool Visited;

		std::vector<IO *> Inputs;
		std::vector<Attribute *> Attributes;
		std::vector<IO *> Outputs;

		std::vector<Edge *> InputEdges;
		std::vector<Edge *> OutputEdges;

		struct Line
		{
			int InputUID = -1;
			int AttributeIndex = -1;
			int OutputUID = -1;
		};
		std::vector<Line> Lines;

		DynamicCodeExecutionEngineInterface *DCEE;
		ActivationEngineInterface *AE;
		LossEngineInterface *LE;

	public:
		void ResetIO();

		bool HasInput(unsigned int UID);

		bool HasOutput(unsigned int UID);

		void ChangeInputUID(unsigned int OldUID, unsigned int NewUID);

		void ChangeOutputUID(unsigned int OldUID, unsigned int NewUID);

		void setXY(int x, int y);

		void setX(int x);

		void setY(int y);

		int getX();

		int getY();

		void SetDCEE(DynamicCodeExecutionEngineInterface *DCEE);

		void SetAE(ActivationEngineInterface *AE);

		void SetLE(LossEngineInterface *LE);

		void Visit();

		void Unvisit();

		bool IsVisited();

		void SetParentGraph(Graph *ParentGraph);

		Graph *GetParentGraph();

		void SetUID(unsigned int UID);

		unsigned int GetUID();

		std::string GetTypeID();

		void SetTypeID(std::string TypeID);

		unsigned int MakeInput(unsigned int TypeID, std::function<void()> DrawFunction);

		unsigned int MakeOutput(unsigned int TypeID, std::function<void()> DrawFunction);

		unsigned int MakeAttribute(Attribute *Attribute);

		void MakeLine(unsigned int Input, unsigned int Attribute, unsigned int Output);

		int GetLineCount();

		std::vector<Line> &GetLines();

		std::vector<Edge *> &GetInputEdges();

		std::vector<Attribute *> &GetAttributes();

		IO *GetInputByUID(unsigned int UID);
		IO *GetInputByLine(unsigned int LineIndex);
		IO *GetOutputByUID(unsigned int UID);
		IO *GetOutputByLine(unsigned int LineIndex);

		std::vector<Edge *> &GetOutputEdges();

		std::vector<IO *> &GetInputs();

		std::vector<IO *> &GetOutputs();

		virtual void Init() = 0;

		virtual void Process(bool Direction) = 0;

		virtual void Update(){};

		virtual void DrawNodeTitle(ImGuiContext *Context);

		virtual void DrawNodeProperties(ImGuiContext *Context);

		virtual NS_Node::NodeInterface *GetInstance() = 0;

		virtual nlohmann::json Serialize();
		virtual void DeSerialize(nlohmann::json data, void *DCEE);

		// virtual destructor
		virtual ~NodeInterface(){};
	};

	class Registrar
	{
	private:
		static std::unordered_map<unsigned int, std::function<NodeInterface *()>> Constructors;
		static std::unordered_map<std::string, unsigned int> TypeIDs;
		static std::unordered_map<unsigned int, std::string> TypeIDsReverse;
	public:
		static Registrar *GetRegistrar();

		static std::unordered_map<unsigned int, std::function<NodeInterface *()>> &GetConstructors()
		{
			return GetRegistrar()->Constructors;
		}

		static std::unordered_map<std::string, unsigned int> &GetTypeIDs()
		{
			return GetRegistrar()->TypeIDs;
		}

		static std::unordered_map<unsigned int, std::string> &GetTypeIDsReverse()
		{
			return GetRegistrar()->TypeIDsReverse;
		}

	private:
		Registrar();

		static unsigned int GetTypeID(std::string TypeID)
		{
			return GetTypeIDs()[TypeID];
		}

		static std::string GetTypeID(unsigned int TypeID)
		{
			return GetTypeIDsReverse()[TypeID];
		}

		static bool TypeIDExists(std::string TypeID)
		{
			return GetTypeIDs().find(TypeID) != GetTypeIDs().end();
		}

		static unsigned int RegisterType(std::string TypeID)
		{
			if (GetTypeIDs().find(TypeID) != GetTypeIDs().end())
			{
				return GetTypeIDs()[TypeID];
			}
			unsigned int UID = std::hash<std::string>{}(TypeID);
			GetTypeIDs()[TypeID] = UID;
			GetTypeIDsReverse()[UID] = TypeID;
			return UID;
		}

		static void RegisterConstructor(unsigned int TypeID, std::function<NodeInterface *()> Constructor)
		{
			if (GetConstructors().find(TypeID) != GetConstructors().end())
				throw std::runtime_error("Constructor already registered");
			GetConstructors()[TypeID] = Constructor;
		}

	public:

		static void RegisterNode(std::string TypeID, std::function<NodeInterface *()> Constructor)
		{
			if (!TypeIDExists(TypeID))
				RegisterConstructor(RegisterType(TypeID), Constructor);
			else
				RegisterConstructor(GetTypeID(TypeID), Constructor);
		}

		static NodeInterface *Construct(unsigned int TypeID)
		{
			return GetConstructors()[TypeID]();
		}
	};

	static Registrar* GetRegistrar();
}