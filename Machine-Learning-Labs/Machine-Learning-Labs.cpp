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
	int sleep_time_in_seconds = 15;
	std::this_thread::sleep_for(std::chrono::seconds(sleep_time_in_seconds));
	//wait for keypress
	std::cin.get();

	DynamicCodeExecutionEngineInterface::ReLoadEngine();
	DynamicCodeExecutionEngineInterface* instance = DynamicCodeExecutionEngineInterface::GetInstance();
	instance->DefaultLoad();
	printf("Loaded Engines\n");

	UIEngineInterface* UIEngine = instance->AddEngineInstance<UIEngineInterface>(instance->GetEngine("UIEngine.dll")->GetInstance<UIEngineInterface>());
	UIEngine->SetDCEEngine(instance);
	UIEngine->Init();
	UIEngine->Run();

	return 0;
}
