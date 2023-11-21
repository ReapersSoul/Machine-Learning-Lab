#pragma once
#include <string>
#include <vector>
#include <boost/dll.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

//opencl
#include <CL/opencl.hpp>

#include "../../EngineInterface/EngineInterface.h"
#include "../LibraryInterface/LibraryInterface.h"
#include "../Languages/LanguageInterface/LanguageInterface.h"
#include "../ScriptInterface/ScriptInterface.h"
#include "../../SerializationEngine/SerializableInterface/SerializableInterface/SerializableInterface.h"
#include "../KernelInterface/KernelInterface.h"

struct GPUPlatform {
	cl::Platform platform;
	struct Device {
		cl::Device device;
		int max_work_group_size;
		Device(cl::Device dev, int max) {
			device = dev;
			max_work_group_size = max;
		}
	};
	std::vector<Device> devices;
	std::vector <cl::Context> contexts;
	std::vector<cl::CommandQueue> queues;
	std::vector<cl::Program> programs;
	std::vector<cl::Kernel> kernels;

	GPUPlatform(cl::Platform plat) {
		platform = plat;
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		contexts.resize(devices.size());
		queues.resize(devices.size());
		for (int i = 0; i < devices.size(); i++) {
			this->devices.push_back({ devices[i], (int)devices[i].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() });
			contexts[i] = cl::Context(devices);
			queues[i]=cl::CommandQueue(contexts[i], devices[i]);
		}
	}

	cl::Kernel& GetKernel(std::string name) {
		for (int i = 0; i < kernels.size(); i++) {
			if (kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>() == name) {
				return kernels[i];
			}
		}
		throw "Kernel not found";
		return kernels[0];
	}

	cl::Kernel& GetKernel(int index) {
		return kernels[index];
	}

	void CreateKernel(int device,std::string name, std::string source) {
		cl::Program::Sources sources;
		sources.push_back({ source.c_str(), source.length() });
		programs.push_back(cl::Program(contexts[device], sources));
		//programs.back().build(devices);
		kernels.push_back(cl::Kernel(programs.back(), name.c_str()));
	}

	void CreateKenelFromFile(int device, std::string name, std::string filepath) {
		cl::Program::Sources sources;
		std::ifstream file(filepath);
		std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		sources.push_back({ source.c_str(), source.length() });
		programs.push_back(cl::Program(contexts[device], sources));
		//programs.back().build(devices);
		kernels.push_back(cl::Kernel(programs.back(), name.c_str()));
	}

	void UnloadKernel(int index) {
		kernels.erase(kernels.begin() + index);
		programs.erase(programs.begin() + index);
	}

	void UnloadKernel(std::string name) {
		for (int i = 0; i < kernels.size(); i++) {
			if (kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>() == name) {
				kernels.erase(kernels.begin() + i);
				programs.erase(programs.begin() + i);
				return;
			}
		}
		throw "Kernel not found";
	}
};

class DynamicCodeExecutionEngineInterface: public SerializableInterface {
protected:
	bool isReloading = false;
	static boost::dll::shared_library* EngineLibrary;
	static DynamicCodeExecutionEngineInterface* (*GetEngineInstance)();
	std::vector<LibraryInterface*> CoreLibs;
	std::vector<LibraryInterface*> Engines;
	std::vector<LibraryInterface*> PluginLibs;
	std::vector<LibraryInterface*> Plugins;
	std::vector<LibraryInterface*> LanguageLibs;
	std::vector<LibraryInterface*> ScriptLibs;
	std::vector<LibraryInterface*> OtherLibs;
	std::vector<LanguageInterface *> Languages;
	std::vector<ScriptInterface *> Scripts;
	std::vector<EngineInterface *> EngineInstances;

	//opencl
	std::vector<GPUPlatform> gpu_platforms;
	/*
	intention is to load all pieces of this application as a dll, script, or kernel
	this will allow for quick and easy development, updating, and testing of new features as well as
	allowing for the user to create their own features and add them to the application if we allow it
	maybe impliment a signing system to allow for the user to sign their code with a key that we provide
	to limit the amount of malicious code that can be run on the system
	*/

public:

	virtual bool IsReloading() = 0;

	//get engine
	template<typename T>
	T* GetEngineInstanceAs(std::string name) {
		for (int i = 0; i < EngineInstances.size(); i++) {
			if (EngineInstances[i]->GetName() == name) {
				return (T*)EngineInstances[i];
			}
		}
		throw "Engine not found";
		return NULL;
	}

	//Add engine
	template<typename T>
	T* AddEngineInstance(EngineInterface* engine) {
		EngineInstances.push_back(engine);
		EngineInstances.back()->SetDCEEngine(this);
		return (T*)engine;
	}

	bool HasEngineInstance(std::string name) {
		for (int i = 0; i < EngineInstances.size(); i++) {
			if (EngineInstances[i]->GetName() == name) {
				return true;
			}
		}
		return false;
	}

	virtual nlohmann::json GetConfig() = 0;

	static void ReLoadEngine() {
		if(EngineLibrary != nullptr) {
			EngineLibrary->unload();
			delete EngineLibrary;
			EngineLibrary = new boost::dll::shared_library("Core/DynamicCodeExecutionEngine.dll");
			GetEngineInstance = EngineLibrary->get<DynamicCodeExecutionEngineInterface * ()>("GetInstance");
		}
		else {
			EngineLibrary = new boost::dll::shared_library("Core/DynamicCodeExecutionEngine.dll");
			GetEngineInstance = EngineLibrary->get<DynamicCodeExecutionEngineInterface * ()>("GetInstance");
		}
	}

	static bool EngineLoaded() {
		return EngineLibrary != nullptr && EngineLibrary->is_loaded()&&GetEngineInstance!=nullptr;
	}

	int GetCoreLibCount() {
		return CoreLibs.size();
	}

	int GetEngineCount() {
		return Engines.size();
	}

	int GetPluginLibCount() {
		return PluginLibs.size();
	}

	int GetPluginCount() {
		return Plugins.size();
	}

	int GetLanguageLibCount() {
		return LanguageLibs.size();
	}

	LibraryInterface* LoadCoreLib(std::string path) {
		//check if already loaded
		for (int i = 0; i < CoreLibs.size(); i++) {
			if (CoreLibs[i]->GetPath() == path) {
				return CoreLibs[i];
			}
		}
		LibraryInterface* lib = new LibraryInterface(path);
		lib->Load();
		if (lib->IsLoaded()) {
			printf("Loaded %s\n", path.c_str());
		}
		CoreLibs.push_back(lib);
		return lib;
	}

	LibraryInterface* LoadEngine(std::string path) {
		//check if already loaded
		for (int i = 0; i < Engines.size(); i++) {
			if (Engines[i]->GetPath() == path) {
				return Engines[i];
			}
		}
		LibraryInterface* lib = new LibraryInterface(path);
		lib->Load();
		if (lib->IsLoaded()) {
			printf("Loaded %s\n", path.c_str());
		}
		Engines.push_back(lib);
		return lib;
	}

	LibraryInterface* LoadPluginLib(std::string path) {
		//check if already loaded
		for (int i = 0; i < PluginLibs.size(); i++) {
			if (PluginLibs[i]->GetPath() == path) {
				return PluginLibs[i];
			}
		}
		LibraryInterface* lib = new LibraryInterface(path);
		lib->Load();
		if (lib->IsLoaded()) {
			printf("Loaded %s\n", path.c_str());
		}
		PluginLibs.push_back(lib);
		return lib;
	}

	LibraryInterface* LoadPlugin(std::string path) {
		//check if already loaded
		for (int i = 0; i < Plugins.size(); i++) {
			if (Plugins[i]->GetPath() == path) {
				return Plugins[i];
			}
		}
		LibraryInterface* lib = new LibraryInterface(path);
		lib->Load();
		if (lib->IsLoaded()) {
			printf("Loaded %s\n", path.c_str());
		}
		Plugins.push_back(lib);
		return lib;
	}

	LibraryInterface* LoadLibrary(std::string path) {
		//check if already loaded
		for (int i = 0; i < OtherLibs.size(); i++) {
			if (OtherLibs[i]->GetPath() == path) {
				return OtherLibs[i];
			}
		}
		for (int i = 0; i < CoreLibs.size(); i++) {
			if (CoreLibs[i]->GetPath() == path) {
				return CoreLibs[i];
			}
		}
		for (int i = 0; i < Engines.size(); i++) {
			if (Engines[i]->GetPath() == path) {
				return Engines[i];
			}
		}
		for (int i = 0; i < PluginLibs.size(); i++) {
			if (PluginLibs[i]->GetPath() == path) {
				return PluginLibs[i];
			}
		}
		for (int i = 0; i < Plugins.size(); i++) {
			if (Plugins[i]->GetPath() == path) {
				return Plugins[i];
			}
		}
		for (int i = 0; i < LanguageLibs.size(); i++) {
			if (LanguageLibs[i]->GetPath() == path) {
				return LanguageLibs[i];
			}
		}
		LibraryInterface* lib = new LibraryInterface(path);
		lib->Load();
		if (lib->IsLoaded()) {
			printf("Loaded %s\n", path.c_str());
		}
		bool isEngine = false;
		bool isPluginLib = false;
		bool isPlugin = false;
		bool isLanguageLib = false;
		bool isScript = false;
		bool isSerializable = false;

		//interrogate library for identifiers
		if (lib->GetLibrary().has("IsSinglton")) {
			if (lib->GetLibrary().get<bool>("IsSinglton")) {
				lib->SetSingleton(true);
			}
		}
		if (lib->GetLibrary().has("IsEngine")) {
			if (lib->GetLibrary().get<bool>("IsEngine")) {
				lib->SetEngine(true);
				isEngine=true;
			}
		}
		if (lib->GetLibrary().has("IsLibrary")) {
			if (lib->GetLibrary().get<bool>("IsLibrary")) {
				lib->SetLibrary(true);
			}
		}
		if (lib->GetLibrary().has("IsPlugin")) {
			if (lib->GetLibrary().get<bool>("IsPlugin")) {
				lib->SetPlugin(true);
				isPlugin=true;
			}
		}
		if (lib->GetLibrary().has("IsLanguage")) {
			if (lib->GetLibrary().get<bool>("IsLanguage")) {
				lib->SetLanguage(true);
				isLanguageLib=true;
			}
		}
		if (lib->GetLibrary().has("IsScript")) {
			if (lib->GetLibrary().get<bool>("IsScript")) {
				lib->SetScript(true);
				isScript=true;
			}
		}
		if (lib->GetLibrary().has("IsSerializable")) {
			if (lib->GetLibrary().get<bool>("IsSerializable")) {
				lib->SetSerializable(true);
				isSerializable=true;
			}
		}

		if (isEngine) {
			Engines.push_back(lib);
		}
		else if (isPluginLib) {
			PluginLibs.push_back(lib);
		}
		else if (isPlugin) {
			Plugins.push_back(lib);
		}
		else if (isLanguageLib) {
			LanguageLibs.push_back(lib);
			LanguageInterface* language = lib->GetInstance<LanguageInterface>();
			Languages.push_back(language);
		}
		else if (isScript) {
			ScriptLibs.push_back(lib);
			ScriptInterface* script = lib->GetInstance<ScriptInterface>();
			Scripts.push_back(script);
		}
		else if (isSerializable) {
			OtherLibs.push_back(lib);
		}
		else {
			OtherLibs.push_back(lib);
		}

		return lib;
	}

	static DynamicCodeExecutionEngineInterface* GetInstance() {
		return GetEngineInstance();
	}

	virtual void RegisterLanguage(LanguageInterface * language) = 0;

	virtual void LoadCoreLibs() = 0;

	virtual void LoadCoreEngines() = 0;

	virtual void LoadPluginLibs() = 0;

	virtual void LoadPlugins() = 0;

	virtual void LoadLanguages() = 0;

	virtual void LoadScripts() = 0;

	virtual void RunScripts() = 0;
	
	LanguageInterface* GetLanguage(std::string name) {
		for (int i = 0; i < Languages.size(); i++) {
			if (Languages[i]->GetName() == name) {
				return Languages[i];
			}
		}
		throw "Language not found";
		return NULL;
	}

	std::vector<LanguageInterface *> GetLanguages() {
		return Languages;
	}

	LibraryInterface* GetCoreLibrary(std::string name) {
		for (int i = 0; i < CoreLibs.size(); i++) {
			if (std::filesystem::path(CoreLibs[i]->GetPath()).filename().string() == name) {
				return CoreLibs[i];
			}
		}
		return nullptr;
	}

	LibraryInterface* GetEngine(std::string name) {
		for (int i = 0; i < Engines.size(); i++) {
			if (std::filesystem::path(Engines[i]->GetPath()).filename().string() == name) {
				return Engines[i];
			}
		}
		return nullptr;
	}

	LibraryInterface* GetPluginLib(std::string name) {
		for (int i = 0; i < PluginLibs.size(); i++) {
			if (std::filesystem::path(PluginLibs[i]->GetPath()).filename().string() == name) {
				return PluginLibs[i];
			}
		}
		return nullptr;
	}

	LibraryInterface* GetPlugin(std::string name) {
		for (int i = 0; i < Plugins.size(); i++) {
			if (std::filesystem::path(Plugins[i]->GetPath()).filename().string() == name) {
				return Plugins[i];
			}
		}
		return nullptr;
	}

	LibraryInterface* GetLanguageLib(std::string name) {
		for (int i = 0; i < LanguageLibs.size(); i++) {
			if (std::filesystem::path(LanguageLibs[i]->GetPath()).filename().string() == name) {
				return LanguageLibs[i];
			}
		}
		return nullptr;
	}

	LibraryInterface* GetOtherLib(std::string name) {
		for (int i = 0; i < OtherLibs.size(); i++) {
			if (std::filesystem::path(OtherLibs[i]->GetPath()).filename().string() == name) {
				return OtherLibs[i];
			}
		}
		return nullptr;
	}

	std::vector<LibraryInterface*> GetCoreLibraries() {
		return CoreLibs;
	}

	std::vector<LibraryInterface*> GetEngines() {
		return Engines;
	}

	std::vector<LibraryInterface*> GetPluginLibraries() {
		return PluginLibs;
	}

	std::vector<LibraryInterface*> GetPlugins() {
		return Plugins;
	}

	std::vector<LibraryInterface*> GetLanguageLibraries() {
		return LanguageLibs;
	}

	std::vector<LibraryInterface*> GetOtherLibraries() {
		return OtherLibs;
	}

	DynamicCodeExecutionEngineInterface() {

	}

	virtual ~DynamicCodeExecutionEngineInterface() {
		//delete libs
		for(int i = 0; i < CoreLibs.size(); i++) {
			delete CoreLibs[i];
		}
	}

	virtual void FullReload() = 0;

	virtual void DefaultLoad() = 0;
};
