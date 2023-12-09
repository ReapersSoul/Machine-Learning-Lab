#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <AttributeInterface.hpp>
#include <string>

class ValueNode : public NodeInterface {
    double value = 0;
public:
    ValueNode() {
        TypeID = "ValueNode";
    }

    void Process(bool DirectionForward) override {
        if (DirectionForward) {
            nlohmann::json data = nlohmann::json::object();
            data["Data"] = value;
            data["Type"] = "Scalar";
            GetOutputDataByIndex(0) = data;
        }
        else {

        }
    }

    void Init() override {
        MakeOutput(0, "Output", "Any", nlohmann::json::array());

        MakeAttribute(0,new AttributeInterface([this]() {
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("Value", &value);
            }));
    }

    //void DrawNodeTitle();

    NodeInterface* GetInstance() {
        ValueNode* node = new ValueNode();
        return node;
    }

    nlohmann::json Serialize() override {
        nlohmann::json data = NodeInterface::Serialize();

        data["value"] = value;

        return data;
    }

    void DeSerialize(nlohmann::json data, void* DCEE) override {
        NodeInterface::DeSerialize(data, DCEE);

        value = data["value"];

        Attributes.push_back(new AttributeInterface([this]() {
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("Value", &value);
            }));
    }
};


extern "C" {
    EXPORT std::string GetTypeID() {
        return "ValueNode";
    }

    // Define a function that returns the result of adding two numbers
    EXPORT NodeInterface* GetInstance() {
        return new ValueNode();
    }
}