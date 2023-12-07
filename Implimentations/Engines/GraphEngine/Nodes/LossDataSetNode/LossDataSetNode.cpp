#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <UIEngineInterface.h>
#include <LossInterface.h>
#include <string>
#include <glm/glm.hpp>

class LossDataSetNode : public NodeInterface {
	std::vector<double> values = { 0 };
	std::vector<double> targets = { 0 };
	std::vector<double> derivatives = { 0 };
	std::vector<double> losses = { 0 };

	LossInterface* Loss;
	int page = 0;
	int chunk_size = 10;
public:
	LossDataSetNode() {
		TypeID = "LossDataSetNode";
	}

	void Process(bool DirectionForward) override {
		if (!DirectionForward) {
			//clear inputs
			GetInputDataByIndex(0).clear();
			GetInputDataByIndex(0) = nlohmann::json::object();
			GetInputDataByIndex(0)["Data"] = nlohmann::json::array();
			GetInputDataByIndex(0)["Type"] = "Vector";
			for (int i = 0; i < values.size(); i++)
			{
				double L = -Loss->CalculateLossDerivative(values[i], targets[i]);
				derivatives[i] = L;
				GetInputDataByIndex(0)["Data"].push_back(derivatives[i]);
			}
		}
		else {
			if (GetInputDataByIndex(0)[0]["Data"].size() != values.size()) {
				values.resize(GetInputDataByIndex(0)[0]["Data"].size());
				targets.resize(GetInputDataByIndex(0)[0]["Data"].size());
				derivatives.resize(GetInputDataByIndex(0)[0]["Data"].size());
				losses.resize(GetInputDataByIndex(0)[0]["Data"].size());
			}

			if (!GetInputDataByIndex(1).is_null()) {
				if (GetInputDataByIndex(1)[0]["Data"].size() != targets.size()) {
					targets.resize(GetInputDataByIndex(1)[0]["Data"].size());
				}
				//copy targets
				for (int i = 0; i < GetInputDataByIndex(1)[0]["Data"].size(); i++)
				{
					targets[i] = GetInputDataByIndex(1)[0]["Data"][i].get<double>();
				}
			}

			for (int i = 0; i < GetInputDataByIndex(0)[0]["Data"].size(); i++)
			{
				values[i] = GetInputDataByIndex(0)[0]["Data"][i].get<double>();
			}

			for (int i = 0; i < values.size(); i++)
			{
				losses[i] = Loss->CalculateLoss(values[i], targets[i]);
			}
		}
	}

	void Init() override {
		MakeInput(0, "Input", "double", nlohmann::json::array());
		MakeInput(1, "Expected", "double", nlohmann::json::array());

		Loss = LE->GetAvailableLosses()[0];

		//loss
		MakeAttribute(0, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::BeginCombo("Loss", Loss->GetName().c_str())) {
				for (int i = 0; i < LE->GetAvailableLosses().size(); i++)
				{
					bool selected = false;
					ImGui::Selectable(LE->GetAvailableLosses()[i]->GetName().c_str(), &selected);
					if (selected) {
						Loss = LE->GetAvailableLosses()[i];
					}
				}
				ImGui::EndCombo();
			}
			}));

		MakeAttribute(1, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			int size = values.size();
			if (ImGui::InputInt("Size", &size)) {
				if (size <= 0) {
					size = 1;
				}
				values.resize(size);
				targets.resize(size);
				derivatives.resize(size);
				losses.resize(size);
			}
			}));

		MakeAttribute(2, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			//get size of container
			ImVec2 size = ImGui::GetItemRectSize();
			ImGui::SliderInt(("Page##" + std::to_string(UID)).c_str(), &page, 0, targets.size() / 10);
			ImGui::SameLine();
			ImGui::InputInt(("Chunk Size##" + std::to_string(UID)).c_str(), &chunk_size);
			//imgui table
			if (ImGui::BeginTable(("LossTable##" + std::to_string(UID)).c_str(), 3)) {
				int max_page = targets.size() / chunk_size;
				int remainder = targets.size() % chunk_size;
				ImGui::TableSetupColumn(("Targets##" + std::to_string(UID)).c_str(), ImGuiTableColumnFlags_WidthFixed, 300.0f);
				ImGui::TableSetupColumn(("Derivatives##" + std::to_string(UID)).c_str(), ImGuiTableColumnFlags_WidthFixed, 300.0f);
				ImGui::TableSetupColumn(("Losses##" + std::to_string(UID)).c_str(), ImGuiTableColumnFlags_WidthFixed, 300.0f);
				ImGui::TableNextRow();
				// Data for Column 1
				ImGui::TableNextColumn();
				//draw a subset of the targets based on the scroll position
				if (page == max_page) {
					for (int i = page * chunk_size; i < (page * chunk_size) + remainder; i++)
					{
						ImGui::Text(("Target [" + std::to_string(i) + "]").c_str());
						ImGui::SameLine();
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(UID)).c_str(), &targets[i]);
						ImGui::PopItemWidth();
					}
				}
				else {
					for (int i = page * chunk_size; i < (page + 1) * chunk_size; i++)
					{
						ImGui::Text(("Target [" + std::to_string(i) + "]").c_str());
						ImGui::SameLine();
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(UID)).c_str(), &targets[i]);
						ImGui::PopItemWidth();
					}
				}

				// Data for Column 2
				ImGui::TableNextColumn();
				//draw a subset of the derivatives based on the scroll position
				if (page == max_page) {
					for (int i = page * chunk_size; i < (page * chunk_size) + remainder; i++)
					{
						ImGui::Text(("Derivative [" + std::to_string(i) + "]").c_str());
						ImGui::SameLine();
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(UID)).c_str(), &derivatives[i]);
						ImGui::PopItemWidth();
					}
				}
				else {
					for (int i = page * chunk_size; i < (page + 1) * chunk_size; i++)
					{
						ImGui::Text(("Derivative [" + std::to_string(i) + "]").c_str());
						ImGui::SameLine();
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(UID)).c_str(), &derivatives[i]);
						ImGui::PopItemWidth();
					}
				}
				// Data for Column 3
				ImGui::TableNextColumn();
				//draw a subset of the losses based on the scroll position
				if (page == max_page) {
					for (int i = page * chunk_size; i < (page * chunk_size) + remainder; i++)
					{
						ImGui::Text(("Loss [" + std::to_string(i) + "]").c_str());
						ImGui::SameLine();
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(UID)).c_str(), &losses[i]);
						ImGui::PopItemWidth();
					}
				}
				else {
					for (int i = page * chunk_size; i < (page + 1) * chunk_size; i++)
					{
						ImGui::Text(("Loss [" + std::to_string(i) + "]").c_str());
						ImGui::SameLine();
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(UID)).c_str(), &losses[i]);
						ImGui::PopItemWidth();
					}
				}
				ImGui::EndTable();
			}
		}));
	}

	//void DrawNodeTitle();

	NodeInterface* GetInstance() {
		LossDataSetNode* node = new LossDataSetNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		return;
	}
};


extern "C" {
	__declspec(dllexport) std::string GetTypeID() {
		return "LossDataSetNode";
	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new LossDataSetNode();
	}
}