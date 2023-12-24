#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <UIEngineInterface.hpp>
#include <string>


class InputDataSetNode : public NS_Node::NodeInterface {
    std::vector<double> values = { 0 };
public:
    InputDataSetNode() {
        TypeID = "InputDataSetNode";
    }

    void Process(bool DirectionForward) override {
        if (DirectionForward) {
            nlohmann::json data = nlohmann::json::object();
            data["Data"] = values;
            data["Type"] = "Vector";
            //GetOutputDataByIndex(0) = data;
        }
    }

    void Init() override {
        //json array
        // Description=nlohmann::json::array();
        // MakeOutput(0, "Output", "double", 0);
        // MakeAttribute(0, new Attribute([this]() {
		// 	ImGui::PushItemWidth(100);
		// 	int size = values.size();
		// 	ImGui::InputInt("Size", &size);
		// 	if (size <= 0) size = 1;
		// 	values.resize(size);
		// 	}));

        // MakeAttribute(1, new Attribute([this]() {
		// 	ImGui::PushItemWidth(100);
        //     for (int i = 0; i < values.size(); i++) {
        //         ImGui::InputDouble(std::to_string(i).c_str(), &values[i]);
        //     }
        //     }));
    }

    //void DrawNodeTitle();

    NS_Node::NodeInterface* GetInstance() {
        InputDataSetNode* node = new InputDataSetNode();
        return node;
    }

    nlohmann::json Serialize() override {
        nlohmann::json data = NS_Node::NodeInterface::Serialize();

        //values
        data["values"] = nlohmann::json::array();
        for (auto value : values) {
			data["values"].push_back(value);
		}

        return data;
    }

    void DeSerialize(nlohmann::json data, void* DCEE) override {
        NS_Node::NodeInterface::DeSerialize(data, DCEE);

		//values
		values.clear();
        for (auto value : data["values"]) {
            values.push_back(value);
        }

        Attributes.push_back(new Attribute([this]() {
            ImGui::PushItemWidth(100);
            int size = values.size();
            ImGui::InputInt("Size", &size);
            if (size <= 0) size = 1;
            values.resize(size);
            }));

        Attributes.push_back(new Attribute([this]() {
            ImGui::PushItemWidth(100);
            for (int i = 0; i < values.size(); i++) {
                ImGui::InputDouble(std::to_string(i).c_str(), &values[i]);
            }
            }));
    }
};


extern "C" {
    EXPORT std::string GetTypeID() {
        return "InputDataSetNode";
	}

    // Define a function that returns the result of adding two numbers
    EXPORT NS_Node::NodeInterface* GetInstance() {
        return new InputDataSetNode();
    }

    EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}