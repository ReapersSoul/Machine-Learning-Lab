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

//dear imgui
#include <imgui_internal.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//imnodes
#include <imnodes.h>

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

	int itteration = -1;
	int MaxItterations = 1000;
	bool ConstantProcess = false;
	bool ConstantTrain = false;
	int last_hovered_id = -1;

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

		//if edge created
		int FirstNode;
		int FirstNodeIO;
		int SecondNode;
		int SecondNodeIO;
		if (ImNodes::IsLinkCreated(&FirstNode, &FirstNodeIO, &SecondNode, &SecondNodeIO)) {
			GraphEngine->GetGraphs()["main"]->GetMutex().lock();

			//create edge
			EdgeInterface* edge = GraphEngine->GetGraphs()["main"]->CreateEdge(FirstNode, SecondNode, FirstNodeIO, SecondNodeIO);
			if (edge != nullptr) {
				GraphEngine->GetGraphs()["main"]->GetNodes()[FirstNode]->GetOutputEdges().push_back(edge);
				GraphEngine->GetGraphs()["main"]->GetNodes()[SecondNode]->GetInputEdges().push_back(edge);
			}

			GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
		}

		//if hovering over edge
		int HoveredEdge;
		if (ImNodes::IsLinkHovered(&HoveredEdge)) {
			//if delete key is pressed
			if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
				GraphEngine->GetGraphs()["main"]->GetMutex().lock();
				//delete edge
				GraphEngine->GetGraphs()["main"]->DeleteEdge(HoveredEdge);
				GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
			}
		}

		//if hovering over node
		int HoveredNode;
		if (ImNodes::IsNodeHovered(&HoveredNode)) {
			//if delete key is pressed
			if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
				GraphEngine->GetGraphs()["main"]->GetMutex().lock();
				//delete edges
				for (auto edge : GraphEngine->GetGraphs()["main"]->GetNodes()[HoveredNode]->GetInputEdges()) {
					GraphEngine->GetGraphs()["main"]->DeleteEdge(edge->GetUID());
				}
				for (auto edge : GraphEngine->GetGraphs()["main"]->GetNodes()[HoveredNode]->GetOutputEdges()) {
					GraphEngine->GetGraphs()["main"]->DeleteEdge(edge->GetUID());
				}

				//delete node
				GraphEngine->GetGraphs()["main"]->DeleteNode(HoveredNode);
				GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
			}
		}

		//if right click
		if (ImGui::IsMouseClicked(1)) {
			//open context menu
			ImGui::OpenPopup("Node Context Menu");
		}

		//if context menu is open
		if (ImGui::BeginPopup("Node Context Menu")) {
			char* SearchText = new char[100] {""};
			ImGui::InputText("Search", SearchText, 100);

				//loop through all available nodes
				for (auto node : GraphEngine->GetAvailableNodes()) {
					if (RegexMatch(std::string(SearchText)+".*", node.TypeID) || std::string(SearchText) == "") {
						//if node is clicked
						if (ImGui::MenuItem(node.TypeID.c_str())) {
							//add node to graph
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							int uuid = GraphEngine->GetGraphs()["main"]->AddNode(node.CreateNode());
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
							ImNodes::SetNodeScreenSpacePos(uuid, ImGui::GetMousePos());
						}
					}
				}
			ImGui::EndPopup();
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

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		window = glfwCreateWindow(800, 600, "Machine Learning Lab", NULL, NULL);
		if (window == NULL)
		{
			printf("Failed to create GLFW window");
		}
		glfwMakeContextCurrent(window);

		glewInit();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
		//enable docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;


		//init imnodes
		ImNodes::CreateContext();
	}

	void Run() override{
		nlohmann::json Theme;

		// render loop
		while (!glfwWindowShouldClose(window))
		{
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
							ImNodes::SetNodeGridSpacePos(node.second->GetUID(), ImVec2(node.second->getX(), node.second->getY()));
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
					std::vector<int> SelectedNodes(ImNodes::NumSelectedNodes());
					if (ImNodes::NumSelectedNodes() > 0) {
						ImNodes::GetSelectedNodes(SelectedNodes.data());
					}
					std::vector<int> SelectedEdges(ImNodes::NumSelectedLinks());
					if (ImNodes::NumSelectedLinks() > 0) {
						ImNodes::GetSelectedLinks(SelectedEdges.data());
					}
					std::thread([&,SelectedNodes,SelectedEdges]() {
						while (GraphEngine->GetGraphs()["main"]->GetMutex().try_lock()) {};
						GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodes, SelectedEdges);
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}).detach();
				}
				if (ImGui::MenuItem("Backward Step")) {
					std::vector<int> SelectedNodes(ImNodes::NumSelectedNodes());
					if (ImNodes::NumSelectedNodes() > 0) {
						ImNodes::GetSelectedNodes(SelectedNodes.data());
					}
					std::vector<int> SelectedEdges(ImNodes::NumSelectedLinks());
					if (ImNodes::NumSelectedLinks() > 0) {
						ImNodes::GetSelectedLinks(SelectedEdges.data());
					}
					std::thread([&, SelectedNodes, SelectedEdges]() {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
						GraphEngine->GetGraphs()["main"]->Process(false, SelectedNodes, SelectedEdges);
						GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}).detach();
				}
				if (ImGui::MenuItem("Full/Train Step")) {
					std::vector<int> SelectedNodes(ImNodes::NumSelectedNodes());
					if (ImNodes::NumSelectedNodes() > 0) {
						ImNodes::GetSelectedNodes(SelectedNodes.data());
					}
					std::vector<int> SelectedEdges(ImNodes::NumSelectedLinks());
					if (ImNodes::NumSelectedLinks() > 0) {
						ImNodes::GetSelectedLinks(SelectedEdges.data());
					}
					std::thread([&, SelectedNodes, SelectedEdges]() {
						GraphEngine->GetGraphs()["main"]->GetMutex().lock();
					GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodes, SelectedEdges);
					GraphEngine->GetGraphs()["main"]->Process(false, SelectedNodes, SelectedEdges);
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
					std::vector<int> SelectedNodes(ImNodes::NumSelectedNodes());
					if (ImNodes::NumSelectedNodes() > 0) {
						ImNodes::GetSelectedNodes(SelectedNodes.data());
					}
					std::vector<int> SelectedEdges(ImNodes::NumSelectedLinks());
					if (ImNodes::NumSelectedLinks() > 0) {
						ImNodes::GetSelectedLinks(SelectedEdges.data());
					}
					std::thread([&, SelectedNodes, SelectedEdges]() {
						if (ConstantProcess) {
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodes, SelectedEdges);
							GraphEngine->GetGraphs()["main"]->GetMutex().unlock();
						}
						}).detach();
				}
				ImGui::Checkbox("Constant Train", &ConstantTrain);
				if (ConstantTrain) {
					std::vector<int> SelectedNodes(ImNodes::NumSelectedNodes());
					if (ImNodes::NumSelectedNodes() > 0) {
						ImNodes::GetSelectedNodes(SelectedNodes.data());
					}
					std::vector<int> SelectedEdges(ImNodes::NumSelectedLinks());
					if (ImNodes::NumSelectedLinks() > 0) {
						ImNodes::GetSelectedLinks(SelectedEdges.data());
					}
					std::thread([&, SelectedNodes, SelectedEdges]() {
						if (ConstantTrain) {
							GraphEngine->GetGraphs()["main"]->GetMutex().lock();
							GraphEngine->GetGraphs()["main"]->Process(true, SelectedNodes, SelectedEdges);
							GraphEngine->GetGraphs()["main"]->Process(false, SelectedNodes, SelectedEdges);
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
			ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImColor(ImVec4(Theme["Title Bar Color"][0], Theme["Title Bar Color"][1], Theme["Title Bar Color"][2], Theme["Title Bar Color"][3])));
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
				ImNodes::BeginNodeEditor();
				std::map<std::string, GraphInterface*> graphs = GraphEngine->GetGraphs();
				std::map<unsigned int, NodeInterface*> nodes = graphs["main"]->GetNodes();
				std::map<unsigned int, EdgeInterface*> edges = graphs["main"]->GetEdges();

				//draw nodes
				for (auto node : nodes) {
					ImNodes::BeginNode(node.second->GetUID());
					ImNodes::BeginNodeTitleBar();
					node.second->DrawNodeTitle(ImGui::GetCurrentContext());
					ImNodes::EndNodeTitleBar();

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

							//apply input color
							ImNodes::PushColorStyle(ImNodesCol_Pin, IoTypeColors[node.second->GetDescription()[i]["Input"]["TypeID"]]);
							ImNodes::BeginInputAttribute(node.second->GetDescription()[i]["Input"]["UID"]);
							ImGui::Text(node.second->GetDescription()[i]["Input"]["Name"].get<std::string>().c_str());
							ImNodes::EndInputAttribute();
							ImNodes::PopColorStyle();
						}
						if (node.second->GetDescription()[i].find("Attribute") != node.second->GetDescription()[i].end()&& node.second->GetDescription()[i].find("Input") != node.second->GetDescription()[i].end()) {
							ImGui::SameLine();
						}
						if (node.second->GetDescription()[i].find("Attribute") != node.second->GetDescription()[i].end()) {
							//pass imgui context to attribute and opengl context to attribute
							node.second->GetAttributes()[node.second->GetDescription()[i]["Attribute"]]->Draw(ImGui::GetCurrentContext(), window);
						}
						if (node.second->GetDescription()[i].find("Output") != node.second->GetDescription()[i].end() && node.second->GetDescription()[i].find("Attribute") != node.second->GetDescription()[i].end()||
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

							//apply output color
							ImNodes::PushColorStyle(ImNodesCol_Pin, IoTypeColors[node.second->GetDescription()[i]["Output"]["TypeID"]]);
							ImNodes::BeginOutputAttribute(node.second->GetDescription()[i]["Output"]["UID"]);
							ImGui::Text(node.second->GetDescription()[i]["Output"]["Name"].get<std::string>().c_str());
							ImNodes::EndOutputAttribute();
							ImNodes::PopColorStyle();
						}
					}
					ImNodes::EndNode();
					node.second->Update();
				}

				//draw links
				for (auto edge : edges)
				{
					ImNodes::Link(edge.second->GetUID(), edge.second->GetFirstIO()["UID"], edge.second->GetSecondIO()["UID"]);
				}


				ImNodes::EndNodeEditor();

				for (auto node : nodes) {
					node.second->setXY(ImNodes::GetNodeGridSpacePos(node.second->GetUID()).x, ImNodes::GetNodeGridSpacePos(node.second->GetUID()).y);
				}
			}
			ImGui::End();

			//Node Search window
			if (ImGui::Begin("Node Search")) {
			}
			ImGui::End();

			//Node Properties window
			if (ImGui::Begin("Node Properties")) {
				int numSelected = ImNodes::NumSelectedNodes();
				int* selected = new int[numSelected];
				ImNodes::GetSelectedNodes(selected);
				if (numSelected > 0) {
					if (GraphEngine->GetGraphs()["main"]->GetNodes().find(selected[0]) != GraphEngine->GetGraphs()["main"]->GetNodes().end()) {
						GraphEngine->GetGraphs()["main"]->GetNodes()[selected[0]]->DrawNodeProperties(ImGui::GetCurrentContext());
					}
				}
				delete[] selected;
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

			//handle input events
			HandleGraphInputEvents();

			//pop window title color
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImNodes::PopColorStyle();

			//clear screen
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(255, 0, 0, 255);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
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