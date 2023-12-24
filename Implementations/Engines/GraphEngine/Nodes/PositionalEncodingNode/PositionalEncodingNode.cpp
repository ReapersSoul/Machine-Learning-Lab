#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>

class PositionalEncodingNode : public NS_Node::NodeInterface {
	std::vector<double> PositionalEncoding(int symbol_index, int embedding_size) {
		std::vector<double> Output;
		Output.push_back(sin(symbol_index));
		Output.push_back(cos(symbol_index));
		for (int i = 1; i < (embedding_size / 2); i++)
		{
			//generate sine and cosine values and widen them based on the symbol index
			Output.push_back(sin((double)(symbol_index + 1) / (double)i));
			Output.push_back(cos((double)(symbol_index + 1) / (double)i));
		}
		return Output;
	}

	int EmbeddingSize = 0;

	std::vector<double> VectorAddition(std::vector<double> a, std::vector<double> b) {
		std::vector<double> Output;
		for (int i = 0; i < a.size(); i++)
		{
			Output.push_back(a[i] + b[i]);
		}
		return Output;
	}

public:
	PositionalEncodingNode() {
		TypeID = "PositionalEncodingNode";
	}

	void Init() override {
		unsigned int input_one=MakeInput(NS_DataObject::GetTypeID("Scalar"), [](){
			ImGui::Text("Input");
		});
		unsigned int output_one=MakeOutput(NS_DataObject::GetTypeID("Scalar"), [](){
			ImGui::Text("Output");
		});

		unsigned int attribute_one=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Embedding Size", &EmbeddingSize);
			}));
	}

	void Process(bool DirectionForward) override {
		printf("Processing Node %d\n", UID);
		if (DirectionForward) {
			//std::vector<double> Input = GetInputDataByIndex(0).get<std::vector<double>>();
			//split input into vectors of size EmbeddingSize
			std::vector<std::vector<double>> InputSplit;
			// for (int i = 0; i < Input.size(); i += EmbeddingSize)
			// {
			// 	std::vector<double> temp;
			// 	for (int j = 0; j < EmbeddingSize; j++)
			// 	{
			// 		temp.push_back(Input[i + j]);
			// 	}
			// 	InputSplit.push_back(temp);
			// }
			//for each vector in InputSplit, run PositionalEncoding
			std::vector<std::vector<double>> OutputSplit;
			for (int i = 0; i < InputSplit.size(); i++)
			{
				OutputSplit.push_back(PositionalEncoding(i, EmbeddingSize));
			}
			//for each vector in OutputSplit, run VectorAddition
			std::vector<std::vector<double>> Output;
			for (int i = 0; i < OutputSplit.size(); i++)
			{
				Output.push_back(VectorAddition(InputSplit[i], OutputSplit[i]));
			}
			//flatten Output
			std::vector<double> OutputFlattened;
			for (int i = 0; i < Output.size(); i++)
			{
				for (int j = 0; j < Output[i].size(); j++)
				{
					OutputFlattened.push_back(Output[i][j]);
				}
			}
			//GetOutputDataByIndex(0) = OutputFlattened;			
		}
		else {
			//GetInputDataByIndex(0) = GetOutputDataByIndex(0);
		}
	}

	//void DrawNodeTitle();

	void Update() override {

	}

	NodeInterface* GetInstance() {
		PositionalEncodingNode* node = new PositionalEncodingNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		return;
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "PositionalEncodingNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new PositionalEncodingNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}