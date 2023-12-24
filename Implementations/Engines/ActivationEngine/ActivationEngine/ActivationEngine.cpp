#include "../../../Util/Exports.hpp"
#include <ActivationEngineInterface.hpp>
#include <filesystem>

class ActivationEngine : public ActivationEngineInterface
{
public:
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
				if (p.path().filename().string() == "ScriptActivation.dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().extension().string() != ".so") {
					continue;
				}
				if (p.path().filename().string() == "ScriptActivation.so") {
					continue;
				}
			#endif
			NS_Activation::Registrar* registrar = NS_Activation::Registrar::GetRegistrar();

			DCEEngine->LoadLibrary(p.path().string())->Register(registrar);
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

			DCEEngine->LoadLibrary(p.path().string())->Register(NS_Activation::Registrar::GetRegistrar());
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
					NS_Activation::ActivationInterface* scriptActivation = DCEEngine->GetOtherLib("ScriptActivation.dll")->GetInstance<NS_Activation::ActivationInterface>();
				#elif defined(__GNUC__)
					if (p.path().extension().string() != language->GetExtension()) {
						continue;
					}
					NS_Activation::ActivationInterface* scriptActivation = DCEEngine->GetOtherLib("libScriptActivation.so")->GetInstance<NS_Activation::ActivationInterface>();
				#endif

				dynamic_cast<ScriptInterface*>(scriptActivation)->SetDCEEngine(DCEEngine);
				dynamic_cast<ScriptInterface*>(scriptActivation)->SetLanguage(language);
				dynamic_cast<ScriptInterface*>(scriptActivation)->SetPath(p.path().string());
				NS_Activation::Registrar::GetRegistrar()->RegisterActivation(scriptActivation->GetName(), scriptActivation);
			}
		}
	}

	NS_Activation::Registrar* GetRegistrar() override {
		return NS_Activation::Registrar::GetRegistrar();
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