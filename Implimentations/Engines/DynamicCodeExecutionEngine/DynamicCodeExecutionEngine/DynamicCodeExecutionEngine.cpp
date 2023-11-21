#include <DynamicCodeExecutionEngineInterface.h>
#include <PluginInterface.h>

#include <iostream>
#include <boost/dll.hpp>
#include <filesystem>
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

			j["Core Libs Load Order"].push_back("brotlicommon.dll");
			j["Core Libs Load Order"].push_back("brotlidec.dll");
			j["Core Libs Load Order"].push_back("bz2d.dll");
			j["Core Libs Load Order"].push_back("zlibd1.dll");
			j["Core Libs Load Order"].push_back("libpng16d.dll");

			j["Excluded Core DLLs"].push_back("DynamicCodeExecutionEngine.dll");

			j["Engines"].push_back("UIEngine.dll");

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

	void LoadPluginLibs() override {
		nlohmann::json j;
		std::ifstream i("Config.json");
		i >> j;
		//if the libs folder doesn't exist then create it
		if (!std::filesystem::exists("Plugins/Libs/")) {
			std::filesystem::create_directory("Plugins/Libs/");
		}

		//find all dlls in the languages folder
		for (auto& p : std::filesystem::directory_iterator("Plugins/Libs/")) {
			//check if dll
			if (p.path().extension().string() != ".dll") {
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

			// Load the DLL
			LibraryInterface* lib = new LibraryInterface(p.path().string());
			lib->Load();

			// Check if the library loaded successfully
			if (!lib->IsLoaded()) {
				std::cerr << "Failed to load the DLL." << std::endl;
				return;
			}

			PluginLibs.push_back(lib);
		}
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

	void LoadCoreLibs() override {
		std::vector<std::string> loadedLibs;
		nlohmann::json j;
		//if the config file doesn't exist then create it
		if (!std::filesystem::exists("Config.json")) {
			std::ofstream o("Config.json");
			
			j["Core Libs Load Order"] = nlohmann::json::array();
			j["ExcludedDLLs"] = nlohmann::json::array();
			j["Engines"] = nlohmann::json::array();

			j["Core Libs Load Order"].push_back("brotlicommon.dll");
			j["Core Libs Load Order"].push_back("brotlidec.dll");
			j["Core Libs Load Order"].push_back("bz2d.dll");
			j["Core Libs Load Order"].push_back("zlibd1.dll");
			j["Core Libs Load Order"].push_back("libpng16d.dll");

			j["ExcludedDLLs"].push_back("DynamicCodeExecutionEngine.dll");

			j["Engines"].push_back("UIEngine.dll");

			o << std::setw(4) << j << std::endl;
			o.close();
		}

		std::ifstream i("Config.json");
		i >> j;

		//if the core libs folder doesn't exist then create it
		if (!std::filesystem::exists("Core/Libs/")) {
			std::filesystem::create_directory("Core/Libs/");
		}

		for (auto& element : j["Core Libs Load Order"]) {
			// Load the DLL			
			LibraryInterface* myLibrary = new LibraryInterface("Core/Libs/" + element.get<std::string>());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL." << std::endl;
				return;
			}

			CoreLibs.push_back(myLibrary);
			loadedLibs.push_back(element.get<std::string>());
		}

		for (auto& p : std::filesystem::directory_iterator("Core/Libs/")) {
			if (p.is_directory()) {
				continue;
			}
			//check if dll
			if (p.path().extension().string() != ".dll") {
				continue;
			}

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
			

			// Load the DLL			
			LibraryInterface* myLibrary = new LibraryInterface(p.path().string());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL." << std::endl;
				return;
			}

			CoreLibs.push_back(myLibrary);
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

			j["Core Libs Load Order"].push_back("brotlicommon.dll");
			j["Core Libs Load Order"].push_back("brotlidec.dll");
			j["Core Libs Load Order"].push_back("bz2d.dll");
			j["Core Libs Load Order"].push_back("zlibd1.dll");
			j["Core Libs Load Order"].push_back("libpng16d.dll");

			j["Excluded DLLs"].push_back("DynamicCodeExecutionEngine.dll");

			j["Engines"].push_back("UIEngine.dll");

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
			if (p.path().extension().string() != ".dll") {
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

			// Load the DLL
			LibraryInterface* myLibrary = new LibraryInterface(p.path().string());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL." << std::endl;
				return;
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
		//find all dlls in the languages folder
		for (auto& p : std::filesystem::directory_iterator("Plugins/Languages/")) {
			//check if dll
			if (p.path().extension().string() != ".dll") {
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

			// Load the DLL
			LibraryInterface* myLibrary = new LibraryInterface(p.path().string());
			myLibrary->Load();

			// Check if the library loaded successfully
			if (!myLibrary->IsLoaded()) {
				std::cerr << "Failed to load the DLL." << std::endl;
				return;
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
		LoadCoreLibs();
		LoadCoreEngines();
		LoadPluginLibs();
		LoadPlugins();
		LoadLanguages();
		RunScripts();
	}
};

static DynamicCodeExecutionEngine instance;

extern "C" {
    // Define a function that returns the result of adding two numbers
    __declspec(dllexport) DynamicCodeExecutionEngineInterface * GetInstance() {
        return &instance;
    }

	__declspec(dllexport) bool IsSinglton() {
		return true;
	}

	__declspec(dllexport) bool IsEngine() {
		return true;
	}

	__declspec(dllexport) bool IsLibrary() {
		return false;
	}

	__declspec(dllexport) bool IsPlugin() {
		return false;
	}

	__declspec(dllexport) bool IsLanguage() {
		return false;
	}

	__declspec(dllexport) bool IsScript() {
		return false;
	}

	__declspec(dllexport) bool IsSerializable() {
		return true;
	}
}
