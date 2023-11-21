#pragma once
#include "../../EngineInterface/EngineInterface.h"
#include <string>
#include <nlohmann/json.hpp>

class UIEngineInterface: public EngineInterface
{
public:

	virtual nlohmann::json GetTheme(std::string) = 0;
	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void* GetWindow() = 0;

	template<typename T>
	T* GetWindowAs() {
		return static_cast<T*>(GetWindow());
	}

	virtual int GetItteration() = 0;
};