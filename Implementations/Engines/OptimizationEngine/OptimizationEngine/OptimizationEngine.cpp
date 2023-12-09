#include "../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <OptimizationEngineInterface.hpp>
#include <filesystem>


class OptimizationEngine : public OptimizationEngineInterface {	
public:
	std::vector<OptimizerInterface*>& GetAvailableOptimizers() override {
		return AvailableOptimizers;
	}

	void LoadOptimizerCore() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}
		if (!std::filesystem::exists("Core/Optimizers/")) {
			std::filesystem::create_directory("Core/Optimizers/");
		}
		for (auto& p : std::filesystem::directory_iterator("Core/Optimizers/")) {
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
			OptimizerInterface* Optimizer = DCEEngine->LoadLibrary(p.path().string())->GetInstance<OptimizerInterface>();

			#if defined(_MSC_VER)
				if (p.path().filename().string() == "ScriptOptimizer.dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().filename().string() == "libScriptOptimizer.so") {
					continue;
				}
			#endif

			AvailableOptimizers.push_back(Optimizer);
		}
	}
	void LoadOptimizerPlugins() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Plugins/")) {
			std::filesystem::create_directory("Plugins/");
		}
		if (!std::filesystem::exists("Plugins/Optimizers/")) {
			std::filesystem::create_directory("Plugins/Optimizers/");
		}
		for (auto& p : std::filesystem::directory_iterator("Plugins/Optimizers/")) {
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

			AvailableOptimizers.push_back(DCEEngine->LoadLibrary(p.path().string())->GetInstance<OptimizerInterface>());
		}
	}

	void LoadOptimizerScripts() override {
		//if the scripts doesn't have a folder then create one
		if (!std::filesystem::exists("Scripts/")) {
			std::filesystem::create_directory("Scripts/");
		}
		//script nodes
		std::vector<LanguageInterface*> languages = DCEEngine->GetLanguages();
		for (auto& language : languages) {
			//if the language doesn't have a folder then create one
			if (!std::filesystem::exists("Scripts/" + language->GetName())) {
				std::filesystem::create_directory("Scripts/" + language->GetName());
			}
			if (!std::filesystem::exists("Scripts/" + language->GetName() + "/Optimizers/")) {
				std::filesystem::create_directory("Scripts/" + language->GetName() + "/Optimizers/");
			}
			for (auto& p : std::filesystem::directory_iterator("Scripts/" + language->GetName() + "/Optimizers")) {
				if (p.is_directory()) {
					continue;
				}
				//check if dll
				#if defined(_MSC_VER)
					if (p.path().extension().string() != ".dll") {
						continue;
					}
					AvailableOptimizers.push_back(DCEEngine->GetOtherLib("ScriptOptimizer.dll")->GetInstance<OptimizerInterface>());
				#elif defined(__GNUC__)
					if (p.path().extension().string() != ".so") {
						continue;
					}
					AvailableOptimizers.push_back(DCEEngine->GetOtherLib("libScriptOptimizer.so")->GetInstance<OptimizerInterface>());
				#endif
				dynamic_cast<ScriptInterface*>(AvailableOptimizers.back())->SetDCEEngine(DCEEngine);
				dynamic_cast<ScriptInterface*>(AvailableOptimizers.back())->SetLanguage(language);
				dynamic_cast<ScriptInterface*>(AvailableOptimizers.back())->SetPath(p.path().string());
			}
		}
	}

	OptimizationEngine() {
		Name = "OptimizationEngine";
	}
};