#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <AttributeInterface.hpp>
#include <string>

class MathNode : public NodeInterface {
    double value = 0;
    int operation = 0;
public:
    MathNode() {
        TypeID = "MathNode";
    }

    void Process(bool DirectionForward) override {
        //if (Inputs[0]->GetData().size() > 0) {
        //    value = 0;
        //    for (int i = 0; i < Inputs[0]->GetData().size(); i++)
        //    {
        //        switch (operation)
        //        {
        //        case 0:
        //            value += Inputs[0]->GetData()[i];
        //            break;
        //        case 1:
        //            value -= Inputs[0]->GetData()[i];
        //            break;
        //        case 2:
        //            value *= Inputs[0]->GetData()[i];
        //            break;
        //        case 3:
        //            value /= Inputs[0]->GetData()[i];
        //            break;
        //        }
        //    }
        //}
        ////clear inputs
        //Outputs[0]->GetData().push_back(value);
    }

    void Init() override {
        ////load script
        Attributes.push_back(new AttributeInterface([this]() {
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("Value", &value);
            }));

        Attributes.push_back(new AttributeInterface([this]() {
            ImGui::PushItemWidth(100);
            //combo box
            const char* items[] = { "Add", "Subtract", "Multiply", "Divide" };
            ImGui::Combo("Operation", &operation, items, IM_ARRAYSIZE(items));
            }));
    }

    void StandAloneInit() {

    }

    //void DrawNodeTitle();

    NodeInterface* GetInstance() {
        MathNode* node = new MathNode();
        return node;
    }

    nlohmann::json Serialize() override {
        nlohmann::json data = NodeInterface::Serialize();

        data["value"] = value;
        data["operation"] = operation;

        return data;
    }

    void DeSerialize(nlohmann::json data, void* DCEE) override {
        NodeInterface::DeSerialize(data, DCEE);

        value = data["value"];

        operation = data["operation"];

        Attributes.push_back(new AttributeInterface([this]() {
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("Value", &value);
            }));

        Attributes.push_back(new AttributeInterface([this]() {
            ImGui::PushItemWidth(100);
            //combo box
            const char* items[] = { "Add", "Subtract", "Multiply", "Divide" };
            ImGui::Combo("Operation", &operation, items, IM_ARRAYSIZE(items));
            }));
        return;
    }
};


extern "C" {
    EXPORT std::string GetTypeID() {
        return "MathNode";
    }

    // Define a function that returns the result of adding two numbers
    EXPORT NodeInterface* GetInstance() {
        return new MathNode();
    }
}