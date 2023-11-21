#include <DynamicCodeExecutionEngineInterface.h>
#include <GraphEngineInterface.h>
#include <ScriptInterface.h>
#include <NodeInterface.h>


#include <filesystem>

class GraphEngine : public GraphEngineInterface {
	struct ScriptNode : public ScriptInterface, public NodeInterface {};
public:
	GraphEngine() {
		Name="GraphEngine";
		
	}
	~GraphEngine() {}

	void LoadAvailableNodes() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}
		if (!std::filesystem::exists("Core/Graph/")) {
			std::filesystem::create_directory("Core/Graph/");
		}
		if (!std::filesystem::exists("Core/Graph/Nodes")) {
			std::filesystem::create_directory("Core/Graph/Nodes");
		}

		//core nodes
		for (auto& p : std::filesystem::directory_iterator("Core/Graph/Nodes/")) {
			if (p.is_directory()) {
				continue;
			}
			//check if dll
			if (p.path().extension().string() != ".dll") {
				continue;
			}

			try {
				LibraryInterface* lib = DCEEngine->LoadLibrary(p.path().string());


				if (p.path().filename().string() == "ScriptNode.dll") {
					continue;
				}
				NodeInfo nodeInfo;
				NodeInterface* node = lib->GetInstance<NodeInterface>();
				nodeInfo.TypeID = node->GetTypeID();
				nodeInfo.CreateNode = [lib]() {return lib->GetInstance<NodeInterface>(); };
				node->~NodeInterface();
				AvailableNodes.push_back(nodeInfo);
			}
			catch (boost::system::system_error& e) {
				printf("Error: %s\n", e.what());
			}
			catch (std::exception e) {
				continue;
			}
		}
	}

	void LoadAvailableScriptNodes() override {
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
			if (!std::filesystem::exists("Scripts/" + language->GetName() + "/Graph/")) {
				std::filesystem::create_directory("Scripts/" + language->GetName() + "/Graph/");
			}
			if (!std::filesystem::exists("Scripts/" + language->GetName() + "/Graph/Nodes")) {
				std::filesystem::create_directory("Scripts/" + language->GetName() + "/Graph/Nodes");
			}
			for (auto& p : std::filesystem::directory_iterator("Scripts/" + language->GetName() + "/Graph/Nodes")) {
				if (p.is_directory()) {
					continue;
				}
				//check if dll
				if (p.path().extension().string() != language->GetExtension()) {
					continue;
				}
				NodeInfo nodeInfo;
				//remove the extension
				nodeInfo.TypeID = p.path().filename().string().substr(0, p.path().filename().string().size() - language->GetExtension().size());
				nodeInfo.CreateNode = [&,language,p]() {
					NodeInterface* scriptNode = DCEEngine->GetOtherLib("ScriptNode.dll")->GetInstance<NodeInterface>();
					dynamic_cast<ScriptInterface*>(scriptNode)->SetDCEEngine(DCEEngine);
					dynamic_cast<ScriptInterface*>(scriptNode)->SetLanguage(language);
					dynamic_cast<ScriptInterface*>(scriptNode)->SetPath(p.path().string());
					return scriptNode;
				};
				AvailableNodes.push_back(nodeInfo);
			}
		}
	}

	void LoadAvailableNodeIOs() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}
		if (!std::filesystem::exists("Core/Graph/")) {
			std::filesystem::create_directory("Core/Graph/");
		}
		if (!std::filesystem::exists("Core/Graph/NodeIOs")) {
			std::filesystem::create_directory("Core/Graph/NodeIOs");
		}
		for (auto& p : std::filesystem::directory_iterator("Core/Graph/NodeIOs/")) {
			if (p.is_directory()) {
				continue;
			}
			//check if dll
			if (p.path().extension().string() != ".dll") {
				continue;
			}


			AvailableNodeIOs.push_back(DCEEngine->LoadLibrary(p.path().string())->GetInstance<NodeIOInterface>());
		}
	}

	void LoadAvailableSorters() override {
		//if the core doesn't have a folder then create one
		if (!std::filesystem::exists("Core/")) {
			std::filesystem::create_directory("Core/");
		}
		if (!std::filesystem::exists("Core/Graph/")) {
			std::filesystem::create_directory("Core/Graph/");
		}
		if (!std::filesystem::exists("Core/Graph/Sorters")) {
			std::filesystem::create_directory("Core/Graph/Sorters");
		}
		for (auto& p : std::filesystem::directory_iterator("Core/Graph/Sorters/")) {
			if (p.is_directory()) {
				continue;
			}
			//check if dll
			if (p.path().extension().string() != ".dll") {
				continue;
			}

			AvailableSorters.push_back(DCEEngine->LoadLibrary(p.path().string())->GetInstance<SorterInterface>());
		}
	}

};


static GraphEngine instance;

extern "C" {
	//__declspec(dllexport) void CleanUp() {
	//	delete &instance;
	//}

	__declspec(dllexport) GraphEngineInterface* GetInstance() {
		return &instance;
	}
}


