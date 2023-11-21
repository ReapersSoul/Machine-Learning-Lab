#pragma once
#include <string>
#include <vector>
#include <boost/dll.hpp>
#include <filesystem>

class LibraryInterface {
protected:
	std::string path;
	boost::dll::shared_library library;
	bool is_Singleton;
	bool is_Engine;
	bool is_library;
	bool is_plugin;
	bool is_language;
	bool is_script;
	bool is_serializable;
public:

	virtual bool IsSingleton() {
		return is_Singleton;
	}

	virtual bool IsEngine() {
		return is_Engine;
	}

	virtual bool IsLibrary() {
		return is_library;
	}

	virtual bool IsPlugin() {
		return is_plugin;
	}

	virtual bool IsLanguage() {
		return is_language;
	}

	virtual bool IsScript() {
		return is_script;
	}

	virtual bool IsSerializable() {
		return is_serializable;
	}

	virtual void SetSingleton(bool singleton) {
		is_Singleton = singleton;
	}

	virtual void SetEngine(bool engine) {
		is_Engine = engine;
	}

	virtual void SetLibrary(bool library) {
		is_library = library;
	}

	virtual void SetPlugin(bool plugin) {
		is_plugin = plugin;
	}

	virtual void SetLanguage(bool language) {
		is_language = language;
	}

	virtual void SetScript(bool script) {
		is_script = script;
	}

	virtual void SetSerializable(bool serializable) {
		is_serializable = serializable;
	}

	LibraryInterface(std::string path) {
		this->path = path;
	}
	virtual std::string GetPath() {
		return path;
	}
	virtual boost::dll::shared_library& GetLibrary() {
		return library;
	}
	virtual void Load() {
		printf("Loading library %s\n", path.c_str());
		try {
			library = boost::dll::shared_library(path);
		}
		catch (boost::system::system_error& e) {
			printf("Error loading library %s\n", path.c_str());
			printf("Error: %s\n", e.what());
		}
	}
	virtual void Unload() {
		printf("Unloading library %s\n", path.c_str());
		//if has cleanup function
		if (library.has("CleanUp")) {
			//call cleanup function
			library.get<void()>("CleanUp")();
		}
		library.unload();
	}
	virtual bool IsLoaded() {
		return library.is_loaded();
	}

	template<typename T>
	T* GetInstance() {
		return library.get<T * ()>("GetInstance")();
	}
};