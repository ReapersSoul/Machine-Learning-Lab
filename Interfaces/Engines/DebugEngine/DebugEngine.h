#pragma once
#include "../EngineInterface/EngineInterface.h"

class DebugEngine : public EngineInterface {
	class Settings {

	} settings;
	class FunctionTimings {

	} function_timings;

public:

	virtual void StartFunctionTimer(std::string function_name) = 0;
	virtual void StopFunctionTimer(std::string function_name) = 0;
};