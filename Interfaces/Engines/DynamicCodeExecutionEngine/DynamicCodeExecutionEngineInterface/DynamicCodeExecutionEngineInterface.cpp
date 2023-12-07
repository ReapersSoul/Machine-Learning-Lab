#include "DynamicCodeExecutionEngineInterface.h"


boost::dll::shared_library* DynamicCodeExecutionEngineInterface::EngineLibrary;
DynamicCodeExecutionEngineInterface* (*DynamicCodeExecutionEngineInterface::GetEngineInstance)();

