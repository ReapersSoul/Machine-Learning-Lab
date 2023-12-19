#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>

class SelfAttentionLayerNode : public NS_Node::NodeInterface {
	//attention layer stuff
	std::vector<std::vector<std::vector<double>>> query_weights;
	std::vector<std::vector<double>> query;
	std::vector<std::vector<double>> key_weights;
	std::vector<std::vector<double>> key;
	std::vector<std::vector<double>> value_weights;
	std::vector<std::vector<double>> value;
	std::vector<std::vector<double>> similarity;
	std::vector<std::vector<double>> attention_output;

	//derivative stuff
	std::vector<std::vector<double>> X;
	std::vector<std::vector<double>> Z;

	//backprop stuff
	double LearningRate = 0.01;


	double DotProduct(std::vector<double> a, std::vector<double> b) {
		double Output = 0;
		for (int i = 0; i < a.size(); i++)
		{
			Output += a[i] * b[i];
		}
		return Output;
	}

	std::vector<double> Forward(std::vector<double> input, std::vector<std::vector<double>> weights) {
		std::vector<double> Output;
		for (int i = 0; i < weights.size(); i++)
		{
			Output.push_back(DotProduct(input, weights[i]));
		}
		return Output;
	}

	std::vector<double> SoftMax(std::vector<double> Input) {
		std::vector<double> Output;
		double Sum = 0;
		for (size_t i = 0; i < Input.size(); i++)
		{
			Sum += exp(Input[i]);
		}
		for (size_t i = 0; i < Input.size(); i++)
		{
			Output.push_back(exp(Input[i]) / Sum);
		}
		return Output;
	}

	//TODO: FIX THIS TO USE ACTUAL DERIVATIVES!!!!!
	std::vector<double> SoftMax_Derivative(std::vector<double> Input) {
		//finite difference method
		std::vector<double> Output;
		double h = 0.0000001;
		for (size_t i = 0; i < Input.size(); i++) 
		{
			std::vector<double> InputPlusH = Input;
			InputPlusH[i] += h;
			Output.push_back((SoftMax(InputPlusH)[i] - SoftMax(Input)[i]) / h);
		}
		return Output;
	}

	double RandRange(double min, double max) {
		return min + (max - min) * (double)rand() / RAND_MAX;
	}

	void ResizeWeights(int NumEmbeddings, int EmbeddingSize) {
		query_weights.resize(NumEmbeddings, std::vector<std::vector<double>>(EmbeddingSize, std::vector<double>(EmbeddingSize)));
		key_weights.resize(NumEmbeddings, std::vector<double>(EmbeddingSize));
		value_weights.resize(NumEmbeddings, std::vector<double>(EmbeddingSize));
		for (int i = 0; i < query_weights.size(); i++)
		{
			for (int j = 0; j < query_weights[i].size(); j++)
			{
				for (int k = 0; k < query_weights[i][j].size(); k++)
				{
					query_weights[i][j][k] = RandRange(-1, 1);
				}
			}
		}
		for (int i = 0; i < key_weights.size(); i++)
		{
			for (int j = 0; j < key_weights[i].size(); j++)
			{
				key_weights[i][j] = RandRange(-1, 1);
			}
		}
		for (int i = 0; i < value_weights.size(); i++)
		{
			for (int j = 0; j < value_weights[i].size(); j++)
			{
				value_weights[i][j] = RandRange(-1, 1);
			}
		}
	}

	void RandomizeWeights() {

	}

	int EmbeddingSize = 2;

public:
	SelfAttentionLayerNode() {
		TypeID = "SelfAttentionLayerNode";
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
			if (ImGui::InputInt("Embedding Size", &EmbeddingSize)){

			}
			}));

		MakeLine(input_one, attribute_one, output_one);
	}

	void Process(bool DirectionForward) override {
		printf("Processing Node %d\n", UID);
		if (DirectionForward) {
			//clear
			query.clear();
			key.clear();
			value.clear();
			similarity.clear();
			attention_output.clear();

			//std::vector<double> Input = GetInputDataByIndex(0)[0].get<std::vector<double>>();
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
			// 	X.push_back(temp);
			// }

			if (InputSplit.size() != query_weights.size()) {
				ResizeWeights(InputSplit.size(), EmbeddingSize);
			}

			//calculate query, key, and value
			for (int i = 0; i < InputSplit.size(); i++) {
				query.push_back(Forward(InputSplit[i], query_weights[i]));
				key.push_back(Forward(InputSplit[i], key_weights));
				value.push_back(Forward(InputSplit[i], value_weights));
			}

			for (int i = 0; i < query.size(); i++)
			{
				std::vector<double> temp;
				for (int j = 0; j < key.size(); j++)
				{
					temp.push_back(DotProduct(query[i], key[j]));
				}
				Z.push_back(temp);
				similarity.push_back(SoftMax(temp));
			}

			//multiply similarity by value
			for (int i = 0; i < similarity.size(); i++)
			{
				std::vector<double> temp;
				for (int j = 0; j < similarity[i].size(); j++)
				{
					temp.push_back(similarity[i][j] * value[j][i]);
				}
				attention_output.push_back(temp);
			}

			//flatten attention_output
			std::vector<double> OutputFlattened;
			for (int i = 0; i < attention_output.size(); i++)
			{
				for (int j = 0; j < attention_output[i].size(); j++)
				{
					OutputFlattened.push_back(attention_output[i][j]);
				}
			}
			//GetOutputDataByIndex(0) = OutputFlattened;
		}
		else {
			std::vector<double> FG;
			// if (GetOutputDataByIndex(0)[0].is_array()) {
			// 	FG= GetOutputDataByIndex(0)[0].get<std::vector<double>>();
			// }
			// else {
			// 	FG.push_back(GetOutputDataByIndex(0)[0].get<double>());
			// }


			for (int i = 0; i < value_weights.size(); i++)
			{
				std::vector<double> Z_Prime = SoftMax_Derivative(Z[i]);
				for (int j = 0; j < value_weights[i].size(); j++)
				{
					value_weights[i][j] += LearningRate * FG[i] * Z_Prime[j] * X[i][j];
					//fix key and query weights
					key_weights[i][j] += LearningRate * FG[i] * Z_Prime[j] * X[i][j];
					for (int k = 0; k < query_weights[i][j].size(); k++)
					{
						query_weights[i][j][k] += LearningRate * FG[i] * Z_Prime[j] * X[i][j] * X[i][k];
						//GetInputDataByIndex(0)[i] = FG[i] * Z_Prime[i] * key_weights[i][j] * value_weights[i][j] * query_weights[i][j][k];
					}
				}
				
			}

		}
	}

	//void DrawNodeTitle();

	void Update() override {

	}

	NodeInterface* GetInstance() {
		SelfAttentionLayerNode* node = new SelfAttentionLayerNode();
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
		return "SelfAttentionLayerNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new SelfAttentionLayerNode();
	}
}