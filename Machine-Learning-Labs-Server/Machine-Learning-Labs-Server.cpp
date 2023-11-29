#include <boost/dll.hpp>
#include <iostream>
#include <string>
#include <DynamicCodeExecutionEngineInterface.h>
#include <NetworkEngineInterface.h>

#include <filesystem>

int main() {
    int SleepTimeInSec = 10;
    std::this_thread::sleep_for(std::chrono::seconds(SleepTimeInSec));
    //wait for keypress
    std::cin.get();

    DynamicCodeExecutionEngineInterface::ReLoadEngine();
    DynamicCodeExecutionEngineInterface* instance = DynamicCodeExecutionEngineInterface::GetInstance();

    instance->DefaultLoad();
    printf("Loaded Engines\n");
    //libs no longer valid (is empty vector) after this point for some reason
    NetworkEngineInterface* NetworkEngine = instance->AddEngineInstance<NetworkEngineInterface>(instance->GetEngine("NetworkEngine.dll")->GetInstance<NetworkEngineInterface>());
    NetworkEngine->SetDCEEngine(instance);
    NetworkEngine->Init(false);
    printf("Starting Network Engine Server\n");
    try {
        NetworkEngine->Run(8080);
    }
    catch (boost::system::system_error e) {
		printf("Error: %s\n", e.what());
	}
    return 0;
}