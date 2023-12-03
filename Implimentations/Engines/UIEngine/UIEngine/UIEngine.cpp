#include <UIEngineInterface.h>
#include <DynamicCodeExecutionEngineInterface.h>
#include <GraphEngineInterface.h>
#include <NodeInterface.h>
#include <AttributeInterface.h>
#include <LibraryInterface.h>
#include <NetworkEngineInterface.h>

//glew
#include <GL/glew.h>
//glfw
#include <GLFW/glfw3.h>

//glm
#include <glm/glm.hpp>

//imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//imgui node editor
#include <imgui-node-editor/imgui_node_editor.h>
#include <imgui-node-editor/imgui_node_editor_internal.h>
namespace ed = ax::NodeEditor;

//json
#include <nlohmann/json.hpp>

#include <string>
#include <regex>

//filesystem	
#include <filesystem>
#include <iostream>

class UIEngine : public UIEngineInterface
{
	GLFWwindow* window;
	GraphEngineInterface* GraphEngine;
	NetworkEngineInterface* NetworkEngine;

	std::string SelectedTheme = "DarkMode";

	ed::Config* config;
	ed::EditorContext* g_Context = nullptr;

	int itteration = -1;
	int MaxItterations = 1000;
	bool ConstantProcess = false;
	bool ConstantTrain = false;
	int last_hovered_id = -1;
	std::string SearchText;

	std::string RegexClean(std::string str) {
		std::regex r("[^a-zA-Z0-9_]");
		return std::regex_replace(str, r, "");
	}

	bool RegexMatch(std::string regex, std::string str) {
		//ignore case
		std::regex r(regex, std::regex_constants::icase);
		return std::regex_match(RegexClean(str), r);
	}

	void HandleGraphInputEvents() {
		try {
			//if right click
			if (ed::ShowBackgroundContextMenu()) {
				//open context menu
				ImGui::OpenPopup("Background Context Menu");
			}

			ed::Suspend();
			static int firstCall = 0;
			static const int maxItterations = 2;
			//if context menu is open
			if (ImGui::BeginPopup("Background Context Menu")) {
				if (firstCall < maxItterations) {
					ImGui::SetWindowPos(ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y));
					if (firstCall < maxItterations) {
						firstCall++;
					}
				}
				//if the context menu is clicked
				char* tmptText = new char[SearchText.size() + 2];
				memcpy_s(tmptText, SearchText.size() + 2, SearchText.c_str(), SearchText.size() + 1);
				ImGui::InputText("Search", tmptText, SearchText.size() + 2);
				SearchText = tmptText;

				//check if SearchText is full of \0
				//loop through all available nodes
				if (SearchText.empty()) {
					for (auto node : GraphEngine->GetAvailableNodes()) {
						//if node is clicked
						if (ImGui::MenuItem(node.TypeID.c_str())) {
							//add node to graph
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							int uuid = GraphEngine->GetGraphs()["main"]->AddNode(node.CreateNode());
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
							ed::SetNodePosition(uuid, ImGui::GetMousePos());
						}
					}
				}
				else {
					for (auto node : GraphEngine->GetAvailableNodes()) {
						if (RegexMatch(SearchText + ".*", node.TypeID)) {
							//if node is clicked
							if (ImGui::MenuItem(node.TypeID.c_str())) {
								//add node to graph
								GraphEngine->GetGraphs()["main"]->GetMutex().lock();
								int uuid = GraphEngine->GetGraphs()["main"]->AddNode(node.CreateNode());
								GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
								ed::SetNodePosition(uuid, ImGui::GetMousePos());
							}
						}
					}
				}
				ImGui::EndPopup();
			}
			else {
				firstCall = 0;
			}

			ed::NodeId ContextNode;
			if (ed::ShowNodeContextMenu(&ContextNode)) {
				//open context menu
				ImGui::OpenPopup("Node Context Menu");
			}

			//if context menu is open
			if (ImGui::BeginPopup("Node Context Menu")) {
				std::vector<ed::NodeId> nodes(ed::GetActionContextSize());
				int nodeCount = ed::GetActionContextNodes(nodes.data(), ed::GetActionContextSize());
				//if node is clicked
				if (ImGui::MenuItem("Delete")) {
					//delete node
					GraphEngine->GetGraphs()["main"]->GetMutex().lock();
					//delete edges
					for (auto edge : GraphEngine->GetGraphs()["main"]->GetNodes()[ContextNode.Get()]->GetInputEdges()) {
						GraphEngine->GetGraphs()["main"]->DeleteEdge(edge->GetUID());
					}
					for (auto edge : GraphEngine->GetGraphs()["main"]->GetNodes()[ContextNode.Get()]->GetOutputEdges()) {
						GraphEngine->GetGraphs()["main"]->DeleteEdge(edge->GetUID());
					}
					//delete node
					GraphEngine->GetGraphs()["main"]->DeleteNode((unsigned int)ContextNode.Get());
					GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
				}
				ImGui::EndPopup();
			}

			//Node Properties window
			if (ImGui::Begin("Node Properties")) {
				if (ed::GetSelectedObjectCount() > 0) {
					std::vector<ed::NodeId> SelectedNodes(ed::GetSelectedObjectCount());
					std::vector<ed::LinkId> SelectedEdges(ed::GetSelectedObjectCount());
					int nodeCount = ed::GetSelectedNodes(SelectedNodes.data(), ed::GetSelectedObjectCount());
					int edgeCount = ed::GetSelectedLinks(SelectedEdges.data(), ed::GetSelectedObjectCount());
					SelectedNodes.resize(nodeCount);
					SelectedEdges.resize(edgeCount);
					std::vector<unsigned int> SelectedNodesUID;
					std::vector<unsigned int> SelectedEdgesUID;
					for (auto node : SelectedNodes) {
						SelectedNodesUID.push_back(node.Get());
					}
					for (auto edge : SelectedEdges) {
						SelectedEdgesUID.push_back(edge.Get());
					}
					if (GraphEngine->GetGraphs()["main"]->GetNodes().find(SelectedNodesUID[0]) != GraphEngine->GetGraphs()["main"]->GetNodes().end()) {
						GraphEngine->GetGraphs()["main"]->GetNodes()[SelectedNodesUID[0]]->DrawNodeProperties(ImGui::GetCurrentContext());
					}
				}
			}
			ImGui::End();

			ed::Resume();

			if (ed::BeginCreate())
			{
				ed::PinId inputPinId, outputPinId;
				if (ed::QueryNewLink(&inputPinId, &outputPinId))
				{
					if (inputPinId && outputPinId) // both are valid, let's accept link
					{
						if (inputPinId != outputPinId)
						{
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							//find the node that the pin belongs to
							unsigned int inputNode = -1;
							unsigned int inputIO = inputPinId.Get();
							unsigned int outputNode = -1;
							unsigned int outputIO = outputPinId.Get();
							for (auto node : GraphEngine->GetGraphs()["main"]->GetNodes()) {
								if (node.second->HasInput(outputIO)) {
									outputNode = node.second->GetUID();
								}
								if (node.second->HasOutput(inputIO)) {
									inputNode = node.second->GetUID();
								}
							}
							if (inputNode != -1 || outputNode != -1) {
								if (ed::AcceptNewItem())
								{
									//create edge
									EdgeInterface* edge = GraphEngine->GetGraphs()["main"]->CreateEdge(inputNode, outputNode, inputIO, outputIO);
									if (edge != nullptr) {
										GraphEngine->GetGraphs()["main"]->GetNodes()[inputNode]->GetOutputEdges().push_back(edge);
										GraphEngine->GetGraphs()["main"]->GetNodes()[outputNode]->GetInputEdges().push_back(edge);
									}
								}
							}
							else {
								ed::RejectNewItem();
							}
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}
					}
				}
			}
			ed::EndCreate();

			if (ed::BeginDelete()) {
				ed::LinkId deletedLinkId;
				while (ed::QueryDeletedLink(&deletedLinkId))
				{
					if (ed::AcceptDeletedItem())
					{
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
						//delete edge
						GraphEngine->GetGraphs()["main"]->DeleteEdge(deletedLinkId.Get());
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
					}
				}

				ed::NodeId deletedNodeId;
				while (ed::QueryDeletedNode(&deletedNodeId)) {
					GraphEngine->GetGraphs()["main"]->GetMutex().lock();
					//delete edges
					for (auto edge : GraphEngine->GetGraphs()["main"]->GetNodes()[deletedNodeId.Get()]->GetInputEdges()) {
						GraphEngine->GetGraphs()["main"]->DeleteEdge(edge->GetUID());
					}
					for (auto edge : GraphEngine->GetGraphs()["main"]->GetNodes()[deletedNodeId.Get()]->GetOutputEdges()) {
						GraphEngine->GetGraphs()["main"]->DeleteEdge(edge->GetUID());
					}

					//delete node
					GraphEngine->GetGraphs()["main"]->DeleteNode((unsigned int)deletedNodeId.Get());
					GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
				}
			}
			ed::EndDelete();

			
		}
		catch (std::exception e) {
			printf("%s\n", e.what());
		}
		catch (...) {
			printf("Unknown error\n");
		}
	}

	std::map<std::string, ImColor> IoTypeColors;
	double ColorDistance = .05;

	ImColor Distance(ImColor Color1, ImColor Color2) {
		return ImColor(abs(Color1.Value.x - Color2.Value.x), abs(Color1.Value.y - Color2.Value.y), abs(Color1.Value.z - Color2.Value.z));
	}

	double RandRange(double min, double max) {
		return min + (max - min) * (double)rand() / RAND_MAX;
	}

	int RandRange(int min, int max) {
		return min + (max - min) * (double)rand() / RAND_MAX;
	}
	std::thread processingthread;

	int mode = 0;
public:

	int GetItteration() override {
		return itteration;
	}

	UIEngine() {
		Name = "UIEngine";
		processingthread = std::thread([&]() {
			while (true) {
				if (itteration >= 0 && itteration < MaxItterations) {

					if (itteration >= 0 && itteration < MaxItterations) {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
						if (mode == 0 || mode == 2) {
							GraphEngine->GetGraphs()["main"]->Process(true);
						}
						if (mode == 1 || mode == 2) {
							GraphEngine->GetGraphs()["main"]->Process(false);
						}
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						itteration++;
					}
				}
				else {
					if (itteration != -1) {
						itteration = -1;
					}
					if (ConstantProcess) {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
						GraphEngine->GetGraphs()["main"]->Process(true);
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
					}
					if (ConstantTrain) {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
						GraphEngine->GetGraphs()["main"]->Process(true);
						GraphEngine->GetGraphs()["main"]->Process(false);
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
					}
				}
			}
			});
		SearchText.resize(100);
		//processingthread.detach();
	}

	~UIEngine()
	{
		std::vector<LibraryInterface*> libs= DCEEngine->GetEngines();
		for (int i = 0; i < libs.size(); i++)
		{
			if (libs[i]->IsEngine()) {
				if (std::filesystem::path(libs[i]->GetPath()).filename().string() == "UIEngine.dll") {
					continue;
				}
				if (libs[i]->GetLibrary().has("CleanUp")) {
					libs[i]->GetLibrary().get<void()>("CleanUp")();
				}
				libs[i]->Unload();
			}
		}
		// Cleanup
		//check if ImGui is initialized
		if (ImGui::GetCurrentContext() != NULL) {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	}

	nlohmann::json GetTheme(std::string ThemeName) override {
		nlohmann::json Theme;
		//try to load theme from file if it exists and if its not opened or being saved by another program
		if (std::filesystem::exists("Themes/" + SelectedTheme + ".json")) {
			if (Theme.is_discarded()) {
				Theme = nlohmann::json();
			}
			//check if theme is valid
			std::ifstream i("Themes/" + SelectedTheme + ".json");
			//if file is open
			if (i) {
				//if file is not empty
				if (i.peek() != std::ifstream::traits_type::eof()) {
					//if file is valid json
					nlohmann::json j;
					i >> j;
					if (!j.is_discarded()) {
						Theme = j;
					}
				}
			}
		}
		else {
			std::filesystem::create_directory("Themes/");
			std::ofstream o("Themes/Default.json");
			o << "{\n\t\"Title Bar Color\": [0.2, 0.2, 0.2, 1],\n\t\"Backround Color\": [0.1, 0.1, 0.1, 1]\n}";
			o.close();
			Theme = nlohmann::json();
			Theme["Title Bar Color"] = { 0.2, 0.2, 0.2, 1 };
			Theme["Backround Color"] = { 0.1, 0.1, 0.1, 1 };
		}
		return Theme;
	}

	void Init() override {
		GraphEngine = DCEEngine->AddEngineInstance<GraphEngineInterface>(DCEEngine->GetEngine("GraphEngine.dll")->GetInstance<GraphEngineInterface>());
		GraphEngine->LoadAvailableNodes();
		GraphEngine->LoadAvailableScriptNodes();
		GraphEngine->LoadAvailableNodeIOs();
		GraphEngine->LoadAvailableSorters();
		GraphEngine->CreateGraph("main");
		GraphEngine->GetGraphs()["main"]->SetSorter(GraphEngine->GetAvailableSorters()[0]);

		//init network engine
		NetworkEngine = DCEEngine->AddEngineInstance<NetworkEngineInterface>(DCEEngine->GetEngine("NetworkEngine.dll")->GetInstance<NetworkEngineInterface>());
		NetworkEngine->SetDCEEngine(DCEEngine);
		NetworkEngine->Init(true);

		// Initialize GLFW
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW!");
		}

		// Create a GLFW window
		window = glfwCreateWindow(800, 600, "ImGui Node Editor Example", nullptr, nullptr);
		if (!window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window!");
		}

		// Make the window's context current
		glfwMakeContextCurrent(window);

		// Initialize GLEW
		if (glewInit() != GLEW_OK) {
			throw std::runtime_error("Failed to initialize GLEW!");
		}

		// Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		// Setup Node Editor
		config=new ed::Config();
		config->SettingsFile = "NodeGraphConfig.json";
		g_Context = ed::CreateEditor(config);


		ImGui::StyleColorsDark();
		//enable docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	void Run() override{
		nlohmann::json Theme;

		// render loop
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			if (DCEEngine->IsReloading()) {
				continue;
			}

			Theme = GetTheme(SelectedTheme);

			//set colors

			//start new frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			//dockspace over viewport
			//push dockspace background color
			ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(Theme["Backround Color"][0], Theme["Backround Color"][1], Theme["Backround Color"][2], Theme["Backround Color"][3]));
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
			ImGui::PopStyleColor();

			//menu bar
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("New")) {
						GraphEngine->CreateGraph("new");
					}
					if (ImGui::MenuItem("Clear")) {
						GraphEngine->GetGraphs()["main"]->Clear();
					}
					if (ImGui::MenuItem("Open")) {
					}
					if (ImGui::MenuItem("Save")) {
						if (std::filesystem::exists("STATE.dat")) {
							std::filesystem::remove("STATE.dat");
						}
						GraphEngine->GetGraphs()["main"]->SerializeToFile("STATE.dat");
					}
					if (ImGui::MenuItem("Load")) {
						GraphEngine->GetGraphs()["main"]->DeSerializeFromFile("STATE.dat", DCEEngine);
						for (auto node : GraphEngine->GetGraphs()["main"]->GetNodes()) {
							ed::SetCurrentEditor(g_Context);
							ed::SetNodePosition(node.second->GetUID(), ImVec2(node.second->getX(), node.second->getY()));
							ed::SetCurrentEditor(nullptr);
						}
					}
					if (ImGui::MenuItem("Save As")) {
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					if (ImGui::MenuItem("Undo")) {
					}
					if (ImGui::MenuItem("Redo")) {
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("View")) {
					if (ImGui::MenuItem("Settings")) {
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help")) {
					if (ImGui::MenuItem("About")) {
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Forward Step")) {
					ed::SetCurrentEditor(g_Context);
					if (ed::GetSelectedObjectCount() > 0) {
						std::vector<ed::NodeId> SelectedNodes(ed::GetSelectedObjectCount());
						std::vector<ed::LinkId> SelectedEdges(ed::GetSelectedObjectCount());
						int nodeCount = ed::GetSelectedNodes(SelectedNodes.data(), ed::GetSelectedObjectCount());
						int edgeCount = ed::GetSelectedLinks(SelectedEdges.data(), ed::GetSelectedObjectCount());
						SelectedNodes.resize(nodeCount);
						SelectedEdges.resize(edgeCount);
						std::vector<unsigned int> SelectedNodesUID;
						std::vector<unsigned int> SelectedEdgesUID;
						for (auto node : SelectedNodes) {
							SelectedNodesUID.push_back(node.Get());
						}
						for (auto edge : SelectedEdges) {
							SelectedEdgesUID.push_back(edge.Get());
						}

						std::thread([&, SelectedNodesUID, SelectedEdgesUID]() {
							while (GraphEngine->GetGraphs()["main"]->GetMutex().try_lock()) {};
							GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodesUID, SelectedEdgesUID);
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
							}).detach();
					}
					ed::SetCurrentEditor(nullptr);
				}
				if (ImGui::MenuItem("Backward Step")) {
					ed::SetCurrentEditor(g_Context);
					std::vector<ed::NodeId> SelectedNodes(ed::GetSelectedObjectCount());
					std::vector<ed::LinkId> SelectedEdges(ed::GetSelectedObjectCount());
					int nodeCount = ed::GetSelectedNodes(SelectedNodes.data(), ed::GetSelectedObjectCount());
					int edgeCount = ed::GetSelectedLinks(SelectedEdges.data(), ed::GetSelectedObjectCount());
					SelectedNodes.resize(nodeCount);
					SelectedEdges.resize(edgeCount);
					std::vector<unsigned int> SelectedNodesUID;
					std::vector<unsigned int> SelectedEdgesUID;
					for (auto node : SelectedNodes) {
						SelectedNodesUID.push_back(node.Get());
					}
					for (auto edge : SelectedEdges) {
						SelectedEdgesUID.push_back(edge.Get());
					}
					std::thread([&, SelectedNodesUID, SelectedEdgesUID]() {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
						GraphEngine->GetGraphs()["main"]->Process(false, SelectedNodesUID, SelectedEdgesUID);
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}).detach();
				}
				if (ImGui::MenuItem("Full/Train Step")) {
					ed::SetCurrentEditor(g_Context);
					std::vector<ed::NodeId> SelectedNodes(ed::GetSelectedObjectCount());
					std::vector<ed::LinkId> SelectedEdges(ed::GetSelectedObjectCount());
					int nodeCount = ed::GetSelectedNodes(SelectedNodes.data(), ed::GetSelectedObjectCount());
					int edgeCount = ed::GetSelectedLinks(SelectedEdges.data(), ed::GetSelectedObjectCount());
					SelectedNodes.resize(nodeCount);
					SelectedEdges.resize(edgeCount);
					std::vector<unsigned int> SelectedNodesUID;
					std::vector<unsigned int> SelectedEdgesUID;
					for (auto node : SelectedNodes) {
						SelectedNodesUID.push_back(node.Get());
					}
					for (auto edge : SelectedEdges) {
						SelectedEdgesUID.push_back(edge.Get());
					}
					std::thread([&, SelectedNodesUID, SelectedEdgesUID]() {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
					GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodesUID, SelectedEdgesUID);
					GraphEngine->GetGraphs()["main"]->Process(false, SelectedNodesUID, SelectedEdgesUID);
					GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
					}).detach();
				}
				if (ImGui::MenuItem("Forward For")) {
					itteration = 0;
					mode = 0;
				}
				if (ImGui::MenuItem("Backward For")) {
					itteration = 0;
					mode = 1;
				}
				if (ImGui::MenuItem("Full/Train For")) {
					itteration = 0;
					mode = 2;
				}
				ImGui::PushItemWidth(100);
				ImGui::InputInt("Itterations", &MaxItterations);
				ImGui::Text("Current Itt: %i", itteration);
				ImGui::Checkbox("Constant Forward", &ConstantProcess);
				if (ConstantProcess) {
					std::vector<ed::NodeId> SelectedNodes(ed::GetSelectedObjectCount());
					std::vector<ed::LinkId> SelectedEdges(ed::GetSelectedObjectCount());
					int nodeCount = ed::GetSelectedNodes(SelectedNodes.data(), ed::GetSelectedObjectCount());
					int edgeCount = ed::GetSelectedLinks(SelectedEdges.data(), ed::GetSelectedObjectCount());
					SelectedNodes.resize(nodeCount);
					SelectedEdges.resize(edgeCount);
					std::vector<unsigned int> SelectedNodesUID;
					std::vector<unsigned int> SelectedEdgesUID;
					for (auto node : SelectedNodes) {
						SelectedNodesUID.push_back(node.Get());
					}
					for (auto edge : SelectedEdges) {
						SelectedEdgesUID.push_back(edge.Get());
					}
					std::thread([&, SelectedNodesUID, SelectedEdgesUID]() {
						if (ConstantProcess) {
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodesUID, SelectedEdgesUID);
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}
						}).detach();
				}
				ImGui::Checkbox("Constant Train", &ConstantTrain);
				if (ConstantTrain) {
					std::vector<ed::NodeId> SelectedNodes(ed::GetSelectedObjectCount());
					std::vector<ed::LinkId> SelectedEdges(ed::GetSelectedObjectCount());
					int nodeCount = ed::GetSelectedNodes(SelectedNodes.data(), ed::GetSelectedObjectCount());
					int edgeCount = ed::GetSelectedLinks(SelectedEdges.data(), ed::GetSelectedObjectCount());
					SelectedNodes.resize(nodeCount);
					SelectedEdges.resize(edgeCount);
					std::vector<unsigned int> SelectedNodesUID;
					std::vector<unsigned int> SelectedEdgesUID;
					for (auto node : SelectedNodes) {
						SelectedNodesUID.push_back(node.Get());
					}
					for (auto edge : SelectedEdges) {
						SelectedEdgesUID.push_back(edge.Get());
					}
					std::thread([&, SelectedNodesUID, SelectedEdgesUID]() {
						if (ConstantTrain) {
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodesUID, SelectedEdgesUID);
							GraphEngine->GetGraphs()["main"]->Process(false, SelectedNodesUID, SelectedEdgesUID);
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}
						}).detach();
				}
				ImGui::EndMainMenuBar();
			}

			//window title bar color
			ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3]));
			ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3]));
			ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3]));
			ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3]));
			ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3]));
			ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3]));
			
			//settings window
			if (ImGui::Begin("Settings")) {
				//reload all engines
				if (ImGui::Button("Reload Engines")) {
					DCEEngine->FullReload();
				}

				//sorters
				ImGui::Text("Available Sorters:");
				std::vector<SorterInterface*> sorters = GraphEngine->GetAvailableSorters();
				if (ImGui::BeginCombo("Sorter", sorters[0]->GetName().c_str())) {
					for (int i = 0; i < sorters.size(); i++)
					{
						bool selected = false;
						ImGui::Selectable(sorters[i]->GetName().c_str(), &selected);
						if (selected) {
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							GraphEngine->GetGraphs()["main"]->SetSorter(sorters[i]);
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}
					}
					ImGui::EndCombo();
				}

				//loop through all json files in the themes folder
				if (ImGui::BeginCombo("Theme", SelectedTheme.c_str())) {
					for (auto file : std::filesystem::directory_iterator("Themes/")) {
						if (file.path().extension().string() == ".json") {
							bool selected = false;
							//get filename without extension
							ImGui::Selectable(file.path().filename().replace_extension("").string().c_str(), &selected);
							if (selected) {
								//check if theme is valid
								std::ifstream i(file.path().string());
								//if file is open
								if (i) {
									//if file is not empty
									if (i.peek() != std::ifstream::traits_type::eof()) {
										//if file is valid json
										nlohmann::json j;
										i >> j;
										if (!j.is_discarded()) {
											SelectedTheme = file.path().filename().replace_extension("").string();
											continue;
										}
									}
								}
								SelectedTheme = "DarkMode";
							}
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Separator();
				ImGui::Text("Available Languages:");
				//languages
				std::vector<LanguageInterface*> languages = DCEEngine->GetLanguages();
				for (int i = 0; i < languages.size(); i++)
				{
					ImGui::Text(languages[i]->GetName().c_str());
				}
				ImGui::Separator();
				ImGui::Text("Available Nodes:");
				//nodes
				std::vector<NodeInfo> nodes = GraphEngine->GetAvailableNodes();
				for (int i = 0; i < nodes.size(); i++)
				{
					ImGui::Text(nodes[i].TypeID.c_str());
				}
				ImGui::Separator();
				ImGui::Text("Available Activations:");
				//nodes
				std::vector<ActivationInterface*> activations = GraphEngine->GetAvailableActivations();
				for (int i = 0; i < activations.size(); i++)
				{
					ImGui::Text(activations[i]->GetName().c_str());
				}
				ImGui::Separator();
				ImGui::Text("Available Losses:");
				//nodes
				std::vector<LossInterface*> losses = GraphEngine->GetAvailableLosses();
				for (int i = 0; i < losses.size(); i++)
				{
					ImGui::Text(losses[i]->GetName().c_str());
				}
			}
			ImGui::End();

			//Node editor window
			if (ImGui::Begin("Node Editor")) {
				ImGui::Separator();
				ed::SetCurrentEditor(g_Context);
				ed::Begin("NodeEditor");


				std::map<std::string, GraphInterface*> graphs = GraphEngine->GetGraphs();
				std::map<unsigned int, NodeInterface*> nodes = graphs["main"]->GetNodes();
				std::map<unsigned int, EdgeInterface*> edges = graphs["main"]->GetEdges();
				//draw nodes
				for (auto node : nodes) {
					ed::BeginNode(node.second->GetUID());
					node.second->DrawNodeTitle(ImGui::GetCurrentContext());
					ImGui::SameLine();
					ImGui::Text(std::to_string(node.second->GetUID()).c_str());

					for (int i = 0; i < node.second->GetDescription().size(); i++) {
						if (node.second->GetDescription()[i].find("Input") != node.second->GetDescription()[i].end()) {
							//get color
							if (IoTypeColors.find(node.second->GetDescription()[i]["Input"]["TypeID"]) == IoTypeColors.end()) {
								ImColor col = ImColor(RandRange(0, 255), RandRange(0, 255), RandRange(0, 255));
								while (true) {
									bool valid = true;
									for (auto color : IoTypeColors) {
										if (Distance(color.second, col).Value.x < ColorDistance && Distance(color.second, col).Value.y < ColorDistance && Distance(color.second, col).Value.z < ColorDistance) {
											valid = false;
											break;
										}
									}
									if (valid) {
										break;
									}
									col = ImColor(RandRange(0, 255), RandRange(0, 255), RandRange(0, 255));
								}
								IoTypeColors[node.second->GetDescription()[i]["Input"]["TypeID"]] = col;
							}

							//TODO:: color and icon
							ed::BeginPin((ed::PinId)node.second->GetInputByIndex(i)["UID"].get<unsigned int>(), ed::PinKind::Input);
							ImGui::Text(node.second->GetInputByIndex(i)["Name"].get<std::string>().c_str());
							ImGui::SameLine();
							ImGui::Text(node.second->GetInputByIndex(i)["UID"].get<std::string>().c_str());
							ed::EndPin();

						}
						if (node.second->GetDescription()[i].find("Attribute") != node.second->GetDescription()[i].end() && node.second->GetDescription()[i].find("Input") != node.second->GetDescription()[i].end()) {
							ImGui::SameLine();
						}
						if (node.second->GetDescription()[i].find("Attribute") != node.second->GetDescription()[i].end()) {
							//pass imgui context to attribute and opengl context to attribute
							node.second->GetAttributes()[node.second->GetDescription()[i]["Attribute"]]->Draw(ImGui::GetCurrentContext(), window);
						}
						if (node.second->GetDescription()[i].find("Output") != node.second->GetDescription()[i].end() && node.second->GetDescription()[i].find("Attribute") != node.second->GetDescription()[i].end() ||
							node.second->GetDescription()[i].find("Output") != node.second->GetDescription()[i].end() && node.second->GetDescription()[i].find("Input") != node.second->GetDescription()[i].end()) {
							ImGui::SameLine();
						}
						if (node.second->GetDescription()[i].find("Output") != node.second->GetDescription()[i].end()) {
							//get color
							if (IoTypeColors.find(node.second->GetDescription()[i]["Output"]["TypeID"]) == IoTypeColors.end()) {
								ImColor col = ImColor(RandRange(0, 255), RandRange(0, 255), RandRange(0, 255));
								while (true) {
									bool valid = true;
									for (auto color : IoTypeColors) {
										if (Distance(color.second, col).Value.x < ColorDistance && Distance(color.second, col).Value.y < ColorDistance && Distance(color.second, col).Value.z < ColorDistance) {
											valid = false;
											break;
										}
									}
									if (valid) {
										break;
									}
									col = ImColor(RandRange(0, 255), RandRange(0, 255), RandRange(0, 255));
								}
								IoTypeColors[node.second->GetDescription()[i]["Output"]["TypeID"]] = col;
							}

							//TODO: color and icon
							ed::BeginPin((ed::PinId)node.second->GetOutputByIndex(i)["UID"].get<unsigned int>(), ed::PinKind::Output);
							ImGui::Text(node.second->GetOutputByIndex(i)["Name"].get<std::string>().c_str());
							ImGui::SameLine();
							ImGui::Text(std::to_string(node.second->GetOutputByIndex(i)["UID"].get<unsigned int>()).c_str());
							ed::EndPin();
						}
					}
					ed::EndNode();
					node.second->Update();
				}
				//draw links
				for (auto edge : edges)
				{
					ed::Link(edge.second->GetUID(), (ed::PinId)edge.second->GetFirstIO()["UID"].get<unsigned int>(), (ed::PinId)edge.second->GetSecondIO()["UID"]);
				}
				for (auto node : nodes) {
					ImVec2 pos = ed::GetNodePosition(node.second->GetUID());
					node.second->setXY(pos.x, pos.y);
				}

				HandleGraphInputEvents();
				ed::End();
			}
			ImGui::End();

			//Node Search window
			if (ImGui::Begin("Node Search")) {
			}
			ImGui::End();

			//Server Console window
			if (ImGui::Begin("Server Console")) {
				ImGui::PushItemWidth(100);
				static std::vector<std::string> logs;
				static std::string command("", 256);

				for (int i = 0; i < logs.size(); i++)
				{
					ImGui::Text(logs[i].c_str());
				}
				ImGui::InputText("Command", &command[0], 256);
				ImGui::SameLine();
				static int selected = -1;
				if (ImGui::Button("Send")) {
					logs.push_back(command);
					//if command==select then select server
					if (command == "select") {
						selected = atoi(command.c_str());
						command = "";
						continue;
					}
					NetworkEngine->GetServer(selected)->Write(command);
					logs.push_back("Server: " + NetworkEngine->GetServer(selected)->Read());
					command = "";
				}
				static char* ip = new char[100] { "" };
				ImGui::InputText("IP", ip, 100);
				static int port = 0;
				ImGui::SameLine();
				ImGui::InputInt("Port", &port,0);
				ImGui::SameLine();
				if (ImGui::Button("Connect")) {
					try {
						selected = NetworkEngine->Connect(ip, port);
					}
					catch (boost::system::system_error& e) {
						logs.push_back(e.what());
					}
					logs.push_back("Connected to " + std::string(ip) + ":" + std::to_string(port) + " as " + std::to_string(selected));
				}
			}
			ImGui::End();

			//pop window title color
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();


			ed::SetCurrentEditor(nullptr);
			//clear screen
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(255, 0, 0, 255);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
		}
	}

	void Start() override {
	}

	void Stop() override {

	}

	void* GetWindow() override {
		return window;
	}
};

static UIEngine instance;

extern "C" {
	
	__declspec(dllexport) void CleanUp() {
		instance.~UIEngine();
	}

	__declspec(dllexport) UIEngineInterface* GetInstance() {
		return &instance;
	}
}