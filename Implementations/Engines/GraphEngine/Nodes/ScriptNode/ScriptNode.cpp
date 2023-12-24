#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <ScriptInterface.hpp>
#include <NodeInterface.hpp>
#include <string>

class ScriptNode : public ScriptInterface,public NS_Node::NodeInterface {
    

    ScriptNode(std::string scriptPath)
    {
        path = scriptPath;
    }
public:
    ScriptNode() {
        this->Language = nullptr;
		this->DCEEngine = nullptr;
        this->TypeID = "ScriptNode";
    }

    void SetDCEEngine(DynamicCodeExecutionEngineInterface* dceEngine) override {
        this->DCEEngine = dceEngine;
    }

    void SetLanguage(LanguageInterface* language) override {
        this->Language = language;
    }

    void SetPath(std::string Path) override {
        path = Path;
        if (Language != nullptr) {
            this->Language->LoadScript(this);
        }
        std::string tid= std::filesystem::path(Path).filename().replace_extension("").string();
        //file name is the type id
        this->TypeID = tid;
    }

    void Init() override {
        if (this->Language != nullptr) {
            this->Language->LoadScript(this);
            this->Language->CreateNode(this);
            this->Language->Node_Init(this);
        }
    }

    void Process(bool forward) override {
        if (this->Language != nullptr) {
            this->Language->LoadScript(this);
            this->Language->Node_Process(this, forward);
		}
	}

    NS_Node::NodeInterface* GetInstance() {
        ScriptNode* node = new ScriptNode();
        if (path != "") {
            node->SetPath(path);
        }
        return node;
    }

    nlohmann::json Serialize() override {
        nlohmann::json data = NS_Node::NodeInterface::Serialize();

        data["Language"] = Language->GetName();

        data["Path"] = path;

        return data;
    }

    void DeSerialize(nlohmann::json data, void* DCEE) override {
        NS_Node::NodeInterface::DeSerialize(data, DCEE);

        Language = DCEEngine->GetLanguage(data["Language"]);

        path = data["Path"];
    }
};


extern "C" {
    EXPORT std::string GetTypeID() {
        return "ScriptNode";
    }

    EXPORT NS_Node::NodeInterface* GetInstance() {
        ScriptNode* node = new ScriptNode();
        return dynamic_cast<NS_Node::NodeInterface*>(node);
    }

    EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}