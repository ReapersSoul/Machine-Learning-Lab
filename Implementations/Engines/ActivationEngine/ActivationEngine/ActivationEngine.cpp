#include "../../../Util/Exports.hpp"
#include <ActivationEngineInterface.hpp>

class ActivationEngine : public ActivationEngineInterface
{
public:
	std::vector<ActivationInterface*>& GetAvailableActivations() override {
		return AvailableActivations;
	}

	void LoadActivationCore() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}
		if (!std::filesystem::exists("Core/Activations/")) {
			std::filesystem::create_directory("Core/Activations/");
		}
		for (auto& p : std::filesystem::directory_iterator("Core/Activations/")) {
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

			ActivationInterface* activation = DCEEngine->LoadLibrary(p.path().string())->GetInstance<ActivationInterface>();
			
			#if defined(_MSC_VER)
				if (p.path().filename().string() == "ScriptActivation.dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().filename().string() == "ScriptActivation.so") {
					continue;
				}
			#endif

			AvailableActivations.push_back(activation);
		}
	}
	void LoadActivationPlugins() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Plugins/")) {
			std::filesystem::create_directory("Plugins/");
		}
		if (!std::filesystem::exists("Plugins/Activations/")) {
			std::filesystem::create_directory("Plugins/Activations/");
		}
		for (auto& p : std::filesystem::directory_iterator("Plugins/Activations/")) {
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

			AvailableActivations.push_back(DCEEngine->LoadLibrary(p.path().string())->GetInstance<ActivationInterface>());
		}
	}

	void LoadActivationScripts() override {
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
			if (!std::filesystem::exists("Scripts/" + language->GetName() + "/Activations/")) {
				std::filesystem::create_directory("Scripts/" + language->GetName() + "/Activations/");
			}
			for (auto& p : std::filesystem::directory_iterator("Scripts/" + language->GetName() + "/Activations")) {
				if (p.is_directory()) {
					continue;
				}
				//check if dll
				#if defined(_MSC_VER)
					if (p.path().extension().string() != language->GetExtension()) {
						continue;
					}
				#elif defined(__GNUC__)
					if (p.path().extension().string() != language->GetExtension()) {
						continue;
					}
				#endif

				AvailableActivations.push_back(DCEEngine->GetOtherLib("ScriptActivation.dll")->GetInstance<ActivationInterface>());
				dynamic_cast<ScriptInterface*>(AvailableActivations.back())->SetDCEEngine(DCEEngine);
				dynamic_cast<ScriptInterface*>(AvailableActivations.back())->SetLanguage(language);
				dynamic_cast<ScriptInterface*>(AvailableActivations.back())->SetPath(p.path().string());
			}
		}
	}

	ActivationEngine() {
		Name= "ActivationEngine";
	}
};

static ActivationEngine instance;

extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT ActivationEngine* GetInstance() {
		return &instance;
	}
}