// Machine-Learning-Lab.cpp : Defines the entry point for the application.
//

#include "Machine-Learning-Lab.h"

int main() {
	//int sleep_time_in_seconds = 15;
	//std::this_thread::sleep_for(std::chrono::seconds(sleep_time_in_seconds));
	////wait for keypress
	//std::cin.get();

	DynamicCodeExecutionEngineInterface::ReLoadEngine();
	DynamicCodeExecutionEngineInterface* instance = DynamicCodeExecutionEngineInterface::GetInstance();
	instance->DefaultLoad();
	printf("Loaded Engines\n");

	#if defined(_MSC_VER)
		if (instance->GetEngine("UIEngine.dll") == nullptr)
		{
			printf("UIEngine.dll not found\n");
			return 1;
		}
	#elif defined(__GNUC__)
		if (instance->GetEngine("libUIEngine.so") == nullptr)
		{
			printf("libUIEngine.so not found\n");
			return 1;
		}
	#endif

	#if defined(_MSC_VER)
		UIEngineInterface* UIEngine = instance->AddEngineInstance<UIEngineInterface>(instance->GetEngine("UIEngine.dll")->GetInstance<UIEngineInterface>());
	#elif defined(__GNUC__)
		UIEngineInterface* UIEngine = instance->AddEngineInstance<UIEngineInterface>(instance->GetEngine("libUIEngine.so")->GetInstance<UIEngineInterface>());
	#endif
	UIEngine->SetDCEEngine(instance);
	UIEngine->Init();
	UIEngine->Run();

	return 0;
}
