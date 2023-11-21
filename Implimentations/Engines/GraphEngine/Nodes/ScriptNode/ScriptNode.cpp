#include <DynamicCodeExecutionEngineInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <ScriptInterface.h>
#include <NodeInterface.h>
#include <string>

class ScriptNode : public ScriptInterface,public NodeInterface {
    

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

    NodeInterface* GetInstance() {
        ScriptNode* node = new ScriptNode();
        if (path != "") {
            node->SetPath(path);
        }
        return node;
    }

    nlohmann::json Serialize() override {
        nlohmann::json data = NodeInterface::Serialize();

        data["Language"] = Language->GetName();

        data["Path"] = path;

        return data;
    }

    void DeSerialize(nlohmann::json data, void* DCEE) override {
        NodeInterface::DeSerialize(data, DCEE);

        Language = DCEEngine->GetLanguage(data["Language"]);

        path = data["Path"];
    }
};


extern "C" {
    __declspec(dllexport) NodeInterface* GetInstance() {
        ScriptNode* node = new ScriptNode();
        return dynamic_cast<NodeInterface*>(node);
    }
}