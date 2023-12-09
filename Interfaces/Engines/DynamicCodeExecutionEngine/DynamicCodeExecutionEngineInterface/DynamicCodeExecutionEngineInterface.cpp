#include "DynamicCodeExecutionEngineInterface.hpp"


boost::dll::shared_library* DynamicCodeExecutionEngineInterface::EngineLibrary;
DynamicCodeExecutionEngineInterface* (*DynamicCodeExecutionEngineInterface::GetEngineInstance)();

