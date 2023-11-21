#include <LossInterface.h>
#include <ScriptInterface.h>
#include <math.h>
#include <filesystem>

class ScriptLoss : public ScriptInterface, public LossInterface
{

    ScriptLoss(std::string scriptPath)
    {
        path = scriptPath;
    }
public:
    ScriptLoss() {
        this->Language = nullptr;
        this->DCEEngine = nullptr;
        this->Name = "ScriptLoss";
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

    double CalculateLoss(double input, double target) override {
        if (this->Language != nullptr) {
			this->Language->LoadScript(this);
			return this->Language->Loss_CalculateLoss(input, target);
		}
        return 0;
    }

    double CalculateLossDerivative(double input, double target) override {
        if (this->Language != nullptr) {
            this->Language->LoadScript(this);
            return this->Language->Loss_CalculateLossDerivative(input, target);
        }
        return 0;
    }

};

extern "C" {
    // Define a function that returns the result of adding two numbers
    __declspec(dllexport) LossInterface* GetInstance() {
        return new ScriptLoss();
    }
}