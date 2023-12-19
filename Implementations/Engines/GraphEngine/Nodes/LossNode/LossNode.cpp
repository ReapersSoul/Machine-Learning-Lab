#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>


class LossNode : public NS_Node::NodeInterface {
	double value = 0;
	double target = 0;
	double derivative = 0;
	double loss = 0;
	NS_Loss::LossInterface* Loss;
public:
	LossNode() {
		TypeID = "LossNode";
	}

	void Process(bool DirectionForward) override {
		if (!DirectionForward) {
			//clear inputs
			derivative = Loss->CalculateLossDerivative(value, target);
			//Inputs[0]->GetData().clear();
			//Inputs[0]->GetData().push_back(derivative);
		}
		else {
			//value = Inputs[0]->GetData()[0].get<double>();
			//Inputs[0]->GetData().clear();
			/*if (Inputs[1]->GetData().size() > 0) {
				target = Inputs[1]->GetData()[0].get<double>();
			}*/
			loss = Loss->CalculateLoss(value, target);
		}
	}

	void Update() override {
	}

	void Init() override {
		//Loss = LE->GetAvailableLosses()[0];
		//loss
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			// if (ImGui::BeginCombo("Loss", LE->GetAvailableLosses()[0]->GetName().c_str())) {
			// 	for (int i = 0; i < LE->GetAvailableLosses().size(); i++)
			// 	{
			// 		bool selected = false;
			// 		ImGui::Selectable(LE->GetAvailableLosses()[i]->GetName().c_str(), &selected);
			// 		if (selected) {
			// 			Loss = LE->GetAvailableLosses()[i];
			// 		}
			// 	}
			// 	ImGui::EndCombo();
			// }
			}));

		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Target", &target);
			}));
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Derivative", &derivative);
			}));
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Loss", &loss);
			}));
	}

	void StandAloneInit() {

	}

	//void DrawNodeTitle();

	NodeInterface* GetInstance() {
		LossNode* node = new LossNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();

		data["value"] = value;

		data["target"] = target;

		data["derivative"] = derivative;

		data["loss"] = loss;

		data["Loss"] = Loss->GetName();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		value = data["value"];

		target = data["target"];

		derivative = data["derivative"];

		loss = data["loss"];

		// for (auto loss : LE->GetAvailableLosses()) {
		// 	if (loss->GetName() == data["Loss"].get<std::string>()) {
		// 		Loss = loss;
		// 	}
		// }

		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			// if (ImGui::BeginCombo("Loss", LE->GetAvailableLosses()[0]->GetName().c_str())) {
			// 	for (int i = 0; i < LE->GetAvailableLosses().size(); i++)
			// 	{
			// 		bool selected = false;
			// 		ImGui::Selectable(LE->GetAvailableLosses()[i]->GetName().c_str(), &selected);
			// 		if (selected) {
			// 			Loss = LE->GetAvailableLosses()[i];
			// 		}
			// 	}
			// 	ImGui::EndCombo();
			// }
			}));

		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Target", &target);
			}));
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Derivative", &derivative);
			}));
		Attributes.push_back(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Loss", &loss);
			}));
		return;
	}


	void SerializeToFile(std::string filepath) {
		return;
	}

	void DeSerializeFromFile(std::string filepath, void* DCEE) {
		return;
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "LossNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new LossNode();
	}
}