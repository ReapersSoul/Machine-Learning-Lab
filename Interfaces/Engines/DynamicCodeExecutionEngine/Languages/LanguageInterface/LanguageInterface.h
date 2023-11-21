#pragma once
#include <string>
#include <vector>

class ScriptInterface;

class LanguageInterface
{
public:
    //object for lua tables and python dicts as well as json objects
    class LanguageObject {
    public:
        virtual ~LanguageObject() {}
        virtual int getInt(const std::string& key) const = 0;
        virtual double getDouble(const std::string& key) const = 0;
        virtual std::string getString(const std::string& key) const = 0;
        virtual bool getBool(const std::string& key) const = 0;

        // Add methods to set objects
        virtual void setObject(const std::string& key, LanguageObject* obj) = 0;
        virtual LanguageObject* getObject(const std::string& key) const = 0;
    };

    //general
    virtual std::string GetExtension() = 0;
    virtual std::string GetName() = 0;
    virtual void SetVariable(const char* name, int value) = 0;
    virtual void SetVariable(const char* name, std::string value) = 0;
    virtual void SetVariable(const char* name, float value) = 0;
    virtual void SetVariable(const char* name, double value) = 0;
    virtual void SetVariable(const char* name, bool value) = 0;
    virtual void SetVariable(const char* name, void* value, std::string type) = 0;

    virtual int GetVariableInt(const char* name) = 0;
    virtual std::string GetVariableString(const char* name) = 0;
    virtual float GetVariableFloat(const char* name) = 0;
    virtual double GetVariableDouble(const char* name) = 0;
    virtual bool GetVariableBool(const char* name) = 0;
    virtual void* GetVariablePointer(const char* name, std::string* type) = 0;

    virtual void LoadScript(ScriptInterface* script) = 0;
    virtual void RunScript(ScriptInterface* script) = 0;

    virtual void RunString(std::string script) = 0;
    virtual void RunFile(std::string path) = 0;

    //Node
    virtual void CreateNode(void* node) = 0;
    virtual void Node_Init(void* node) = 0;
    virtual void Node_Process(void* node,bool forward) = 0;
    //Loss
    virtual double Loss_CalculateLoss(double input, double target) = 0;
    virtual double Loss_CalculateLossDerivative(double input, double target) = 0;
    //Activation
    virtual double Activation_Activate(double input) = 0;
    virtual double Activation_ActivateDerivative(double input) = 0;

    //call a function in the script
    virtual void CallFunction(const char* name, std::vector<void*> args, std::vector<std::string> types, void* ret) = 0;

};