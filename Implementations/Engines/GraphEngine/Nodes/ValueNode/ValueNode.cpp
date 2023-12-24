#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>

class ValueNode : public NS_Node::NodeInterface {
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
            //GetOutputDataByIndex(0) = data;
        }
        else {

        }
    }

    void Init() override {
        // MakeOutput(0, "Output", "Any", nlohmann::json::array());

        // MakeAttribute(0,new Attribute([this]() {
        //     ImGui::PushItemWidth(100);
        //     ImGui::InputDouble("Value", &value);
        //     }));
    }

    //void DrawNodeTitle();

    NS_Node::NodeInterface* GetInstance() {
        ValueNode* node = new ValueNode();
        return node;
    }

    nlohmann::json Serialize() override {
        nlohmann::json data = NS_Node::NodeInterface::Serialize();

        data["value"] = value;

        return data;
    }

    void DeSerialize(nlohmann::json data, void* DCEE) override {
        NS_Node::NodeInterface::DeSerialize(data, DCEE);

        value = data["value"];

        Attributes.push_back(new Attribute([this]() {
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
    EXPORT NS_Node::NodeInterface* GetInstance() {
        return new ValueNode();
    }

    EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}