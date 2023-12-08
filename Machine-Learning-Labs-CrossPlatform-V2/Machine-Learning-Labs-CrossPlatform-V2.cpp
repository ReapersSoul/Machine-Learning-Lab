﻿// Machine-Learning-Labs-CrossPlatform-V2.cpp : Defines the entry point for the application.
//

#include "Machine-Learning-Labs-CrossPlatform-V2.h"

#include <boost/dll.hpp>
#include <iostream>
#include <string>
#include <DynamicCodeExecutionEngineInterface.h>
#include <UIEngineInterface.h>
#include <SerializableInterface.h>

#include <NetworkEngineInterface.h>

#include <filesystem>
#include <thread>

int main() {
	//int sleep_time_in_seconds = 15;
	//std::this_thread::sleep_for(std::chrono::seconds(sleep_time_in_seconds));
	////wait for keypress
	//std::cin.get();

	DynamicCodeExecutionEngineInterface::ReLoadEngine();
	DynamicCodeExecutionEngineInterface* instance = DynamicCodeExecutionEngineInterface::GetInstance();
	instance->DefaultLoad();
	printf("Loaded Engines\n");

	if (instance->GetEngine("UIEngine.dll") == nullptr)
	{
		printf("UIEngine.dll not found\n");
		return 1;
	}

	UIEngineInterface* UIEngine = instance->AddEngineInstance<UIEngineInterface>(instance->GetEngine("UIEngine.dll")->GetInstance<UIEngineInterface>());
	UIEngine->SetDCEEngine(instance);
	UIEngine->Init();
	UIEngine->Run();

	return 0;
}