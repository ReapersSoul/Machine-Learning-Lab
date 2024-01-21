#include "../../../Util/Exports.hpp"
#include <LossEngineInterface.hpp>

class LossEngine : public LossEngineInterface
{
public:

	LossEngine() {
		Name = "LossEngine";
	}

	void LoadLossCore() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}
		if (!std::filesystem::exists("Core/Losses/")) {
			std::filesystem::create_directory("Core/Losses/");
		}
		for (auto& p : std::filesystem::directory_iterator("Core/Losses/")) {
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
			NS_Loss::LossInterface* loss = DCEEngine->LoadLibrary(p.path().string())->GetInstance<NS_Loss::LossInterface>();

			#if defined(_MSC_VER)
				if (p.path().filename().string() == "ScriptLoss.dll") {
					continue;
				}
			#elif defined(__GNUC__)
				if (p.path().filename().string() == "libScriptLoss.so") {
					continue;
				}
			#endif

			DCEEngine->LoadLibrary(p.path().string())->Register(GetRegistrar());
		}
	}
	void LoadLossPlugins()override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Plugins/")) {
			std::filesystem::create_directory("Plugins/");
		}
		if (!std::filesystem::exists("Plugins/Losses/")) {
			std::filesystem::create_directory("Plugins/Losses/");
		}
		for (auto& p : std::filesystem::directory_iterator("Plugins/Losses/")) {
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

			DCEEngine->LoadLibrary(p.path().string())->Register(GetRegistrar());
		}
	}
	void LoadLossScripts() override {
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
			if (!std::filesystem::exists("Scripts/" + language->GetName() + "/Losses/")) {
				std::filesystem::create_directory("Scripts/" + language->GetName() + "/Losses/");
			}
			for (auto& p : std::filesystem::directory_iterator("Scripts/" + language->GetName() + "/Losses")) {
				if (p.is_directory()) {
					continue;
				}
				//check if dll
				#if defined(_MSC_VER)
					if (p.path().extension().string() != language->GetExtension()) {
						continue;
					}
					NS_Loss::LossInterface* scriptLoss = DCEEngine->GetOtherLib("ScriptLoss.dll")->GetInstance<NS_Loss::LossInterface>();
				#elif defined(__GNUC__)
					if (p.path().extension().string() != language->GetExtension()) {
						continue;
					}
					NS_Loss::LossInterface* scriptLoss = DCEEngine->GetOtherLib("libScriptLoss.so")->GetInstance<NS_Loss::LossInterface>();
				#endif
				
				dynamic_cast<ScriptInterface*>(scriptLoss)->SetDCEEngine(DCEEngine);
				dynamic_cast<ScriptInterface*>(scriptLoss)->SetLanguage(language);
				dynamic_cast<ScriptInterface*>(scriptLoss)->SetPath(p.path().string());
				Registrar.RegisterLoss(scriptLoss->GetName(), scriptLoss);
			}
		}
	}
};

static LossEngine instance;

extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT LossEngine* GetInstance() {
		return &instance;
	}
}