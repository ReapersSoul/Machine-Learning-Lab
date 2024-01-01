#include "../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <PluginInterface.hpp>

#include <iostream>
#include <boost/dll.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>


class DynamicCodeExecutionEngine : public DynamicCodeExecutionEngineInterface {

public:

	DynamicCodeExecutionEngine() {
		//setup gpu
		//get platforms
		//std::vector<cl::Platform> platforms;
		//cl::Platform::get(&platforms);
		//for (int i = 0; i < platforms.size(); i++) {
		//	gpu_platforms.push_back(GPUPlatform(platforms[i]));
		//}
	}

	bool IsReloading() override {
		return isReloading;
	}

	nlohmann::json GetConfig() override {
		nlohmann::json j;
		//if the config file doesn't exist then create it
		if (!std::filesystem::exists("Config.json")) {
			std::ofstream o("Config.json");

			j["Core Libs Load Order"] = nlohmann::json::array();
			j["Excluded Core DLLs"] = nlohmann::json::array();
			j["Engines"] = nlohmann::json::array();
			j["Core Libs Load Order"]= nlohmann::json::array();

			#if defined(_MSC_VER)
				j["Excluded Core DLLs"].push_back("DynamicCodeExecutionEngine.dll");
				j["Engines"].push_back("UIEngine.dll");
			#elif defined(__GNUC__)
				j["Excluded Core DLLs"].push_back("libDynamicCodeExecutionEngine.so");
				j["Engines"].push_back("libUIEngine.so");
			#endif
			

			o << std::setw(4) << j << std::endl;
			o.close();
		}

		std::ifstream i("Config.json");
		i >> j;

		return j;
	}

    void RegisterLanguage(LanguageInterface* language) override{
		std::cout << "Registering language..." << std::endl;
	}


	void LoadPlugins() override{
		nlohmann::json j;
		std::ifstream i("Config.json");
		i >> j;
		//if Plugins Folder doesn't exist then create it
		if (!std::filesystem::exists("Plugins/")) {
			std::filesystem::create_directory("Plugins/");
		}
	}

	void LoadLibs() override {
		std::vector<std::string> loadedLibs;
		nlohmann::json j;
		//if the config file doesn't exist then create it
		if (!std::filesystem::exists("./Config.json")) {
			std::ofstream o("./Config.json");
			
			j["Libs Load Order"] = nlohmann::json::array();
			j["ExcludedDLLs"] = nlohmann::json::array();
			j["Engines"] = nlohmann::json::array();
			#if defined(_MSC_VER)
				j["ExcludedDLLs"].push_back("DynamicCodeExecutionEngine.dll");
				j["Engines"].push_back("UIEngine.dll");
			#elif defined(__GNUC__)
				j["ExcludedDLLs"].push_back("libDynamicCodeExecutionEngine.so");
				j["Engines"].push_back("libUIEngine.so");
			#endif

			o << std::setw(4) << j << std::endl;
			o.close();
		}

		std::ifstream i("./Config.json");
		i >> j;


		//if the core libs folder doesn't exist then create it
		if (!std::filesystem::exists("Libs/")) {
			std::filesystem::create_directory("Libs/");
		}

		for (auto& element : j["Libs Load Order"]) {
			// Load the DLL			
			LibraryInterface* myLibrary = new LibraryInterface("Libs/" + element.get<std::string>());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL. Libs/" + element.get<std::string>() << std::endl;
				continue;
			}

			OtherLibs.push_back(myLibrary);
			loadedLibs.push_back(element.get<std::string>());
		}

		for (auto& p : std::filesystem::directory_iterator("Libs/")) {
			if (p.is_directory()) {
				continue;
			}
			//check if dll
			#if defined(_MSC_VER)
				if (p.path().extension().string() != ".dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().extension().string() != ".so") {
					continue;
				}
			#endif

			bool alreadyLoaded = false;
			for (auto& loadedLib : loadedLibs) {
				if (p.path().filename().string() == loadedLib) {
					alreadyLoaded = true;
					break;
				}
			}
			if (alreadyLoaded) {
				continue;
			}

			//check if the dll is in the excluded list
			bool excluded = false;
			for (auto& element : j["Excluded DLLs"]) {
				if (p.path().filename().string() == element.get<std::string>()) {
					excluded = true;
					break;
				}
			}
			if (excluded) {
				continue;
			}
			
			printf("Loading %s\n", std::filesystem::absolute(p.path()).string().c_str());
			// Load the DLL			
			LibraryInterface* myLibrary = new LibraryInterface(p.path().string());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL. " + std::filesystem::absolute(p.path()).string() << std::endl;
				continue;
			}

			OtherLibs.push_back(myLibrary);
		}
	}

	void LoadCoreEngines() override {
		std::vector<std::string> loadedLibs;
		nlohmann::json j;
		//if the config file doesn't exist then create it
		if (!std::filesystem::exists("Config.json")) {
			std::ofstream o("Config.json");

			j["Core Libs Load Order"] = nlohmann::json::array();
			j["Excluded DLLs"] = nlohmann::json::array();
			j["Engines"] = nlohmann::json::array();

			#if defined(_MSC_VER)
				j["Excluded DLLs"].push_back("DynamicCodeExecutionEngine.dll");
				j["Engines"].push_back("UIEngine.dll");
			#elif defined(__GNUC__)
				j["Excluded DLLs"].push_back("libDynamicCodeExecutionEngine.so");
				j["Engines"].push_back("libUIEngine.so");
			#endif

			o << std::setw(4) << j << std::endl;
			o.close();
		}

		std::ifstream i("Config.json");
		i >> j;

		//if the core folder doesn't exist then create it
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}

		for (auto& p : std::filesystem::directory_iterator("Core/")) {
			if (p.is_directory()) {
				continue;
			}
			//check if dll
			#if defined(_MSC_VER)
				if (p.path().extension().string() != ".dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().extension().string() != ".so") {
					continue;
				}
			#endif

			//check if the dll is in the excluded list
			bool excluded = false;
			for (auto& element : j["Excluded DLLs"]) {
				if (p.path().filename().string() == element.get<std::string>()) {
					excluded = true;
					break;
				}
			}
			if (excluded) {
				continue;
			}

			// Load the DLL
			LibraryInterface* myLibrary = new LibraryInterface(p.path().string());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL. " + p.path().string() << std::endl;
				continue;
			}

			//check if the dll is in the engine dlls list
			bool engineDll = false;
			for (auto& element : j["Engines"]) {
				if (p.path().filename().string() == element.get<std::string>()) {
					engineDll = true;
					break;
				}
			}
			if (engineDll) {
				myLibrary->GetInstance<void*>(); //this is a hack to make sure the engine is loaded
				myLibrary->SetEngine(true);
			}

			Engines.push_back(myLibrary);
		}

	}

	void LoadLanguages() override{
		nlohmann::json j;
		std::ifstream i("Config.json");
		i >> j;

		//if the plugins folder doesn't exist then create it
		if (!std::filesystem::exists("Plugins/")) {
			std::filesystem::create_directory("Plugins/");
		}

		//if the languages folder doesn't exist then create it
		if (!std::filesystem::exists("Plugins/Languages/")) {
			std::filesystem::create_directory("Plugins/Languages/");
		}

		//find all dlls in the languages folder
		for (auto& p : std::filesystem::directory_iterator("Plugins/Languages/")) {
			//check if dll
			#if defined(_MSC_VER)
				if (p.path().extension().string() != ".dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().extension().string() != ".so") {
					continue;
				}
			#endif

			//check if the dll is in the excluded list
			bool excluded = false;
			for (auto& element : j["Excluded DLLs"]) {
				if (p.path().filename().string() == element.get<std::string>()) {
					excluded = true;
					break;
				}
			}
			if (excluded) {
				continue;
			}

			// Load the DLL
			LibraryInterface* myLibrary = new LibraryInterface(p.path().string());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL. Plugins/Languages/" + p.path().filename().string() << std::endl;
				continue;
			}

			// Get a instance of the class
			Languages.push_back(myLibrary->GetInstance<LanguageInterface>());
		}
	}

	void LoadScripts() override{
		//for each language find the language's folder in the scripts folder
		for (auto& language : Languages) {
			//if the language doesn't have a folder then create one
			if (!std::filesystem::exists("Scripts/" + language->GetName())) {
				std::filesystem::create_directory("Scripts/" + language->GetName());
				continue;
			}
			for (auto& p : std::filesystem::directory_iterator("Scripts/" + language->GetName())) {
				ScriptInterface script;
				script.SetPath(p.path().string());
				script.SetLanguage(language);
				language->LoadScript(&script);
			}
		}
	}

	void RunScripts() override {
		//for each language find the language's folder in the scripts folder
		for (auto& language : Languages) {
			for (auto script : Scripts)
			{
				language->RunScript(script);
			}
		}
	}

	void FullReload() override {
		isReloading = true;
		//reload all the libs that aren't engine libs or in the lib folder
		for (auto& lib : Engines) {
			if (lib->IsEngine()) {
				continue;
			}
			//if it is in a lib folder then continue
			if (lib->GetPath().find("Libs") != std::string::npos) {
				continue;
			}

			lib->Unload();
			lib->Load();
		}
		////reload all the engine libs
		//for (auto& lib : libs) {
		//	if (!lib->GetIsEngineLibrary()) {
		//		continue;
		//	}

		//	lib->Unload();
		//	lib->Load();
		//}
		////reload all the libs in the lib folder
		//for (auto& lib : libs) {
		//	//if it is not in a lib folder then continue
		//	if (lib->GetPath().find("Libs") == std::string::npos) {
		//		continue;
		//	}

		//	lib->Unload();
		//	lib->Load();
		//}

		isReloading = false;
	}


	nlohmann::json Serialize() override {
		return nullptr;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		return;
	}

	void DefaultLoad() {
		LoadLibs();
		LoadCoreEngines();
		LoadPlugins();
		LoadLanguages();
		RunScripts();
	}
};

static DynamicCodeExecutionEngine instance;




extern "C" {
    // Define a function that returns the result of adding two numbers
    EXPORT DynamicCodeExecutionEngineInterface * GetInstance() {
        return &instance;
    }

	EXPORT bool IsSinglton() {
		return true;
	}

	EXPORT bool IsEngine() {
		return true;
	}

	EXPORT bool IsLibrary() {
		return false;
	}

	EXPORT bool IsPlugin() {
		return false;
	}

	EXPORT bool IsLanguage() {
		return false;
	}

	EXPORT bool IsScript() {
		return false;
	}

	EXPORT bool IsSerializable() {
		return true;
	}
}
