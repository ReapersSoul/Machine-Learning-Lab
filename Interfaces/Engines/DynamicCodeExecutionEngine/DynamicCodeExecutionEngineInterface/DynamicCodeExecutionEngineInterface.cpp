#include "DynamicCodeExecutionEngineInterface.h"


boost::dll::shared_library* DynamicCodeExecutionEngineInterface::EngineLibrary;
DynamicCodeExecutionEngineInterface* (*DynamicCodeExecutionEngineInterface::GetEngineInstance)();
//std::vector<LibraryInterface*> DynamicCodeExecutionEngineInterface::libs;
//std::vector<LanguageInterface *> DynamicCodeExecutionEngineInterface::languages;
//std::vector<ScriptInterface *> DynamicCodeExecutionEngineInterface::scripts;

