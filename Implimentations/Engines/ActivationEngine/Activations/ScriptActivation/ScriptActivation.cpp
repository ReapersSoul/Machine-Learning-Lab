#include <ActivationInterface.h>
#include <ScriptInterface.h>
#include <math.h>
#include <filesystem>

class ScriptActivation : public ScriptInterface, public ActivationInterface
{

    ScriptActivation(std::string scriptPath)
    {
		path = scriptPath;
	}
public:
    ScriptActivation() {
        this->Language = nullptr;
        this->DCEEngine = nullptr;
        this->Name = "ScriptActivation";
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
        std::string tid = std::filesystem::path(Path).filename().replace_extension("").string();
        //file name is the type id
        this->Name = tid;
    }

    double Activate(double input) {
        if (this->Language != nullptr) {
            this->Language->LoadScript(this);
            return this->Language->Activation_Activate(input);
        }
        return 0;
    }

    double ActivateDerivative(double input) {
        if (this->Language != nullptr) {
            this->Language->LoadScript(this);
            return this->Language->Activation_ActivateDerivative(input);
        }
        return 0;
    }
};

extern "C" {
    // Define a function that returns the result of adding two numbers
    __declspec(dllexport) ActivationInterface* GetInstance() {
        return new ScriptActivation();
    }
}