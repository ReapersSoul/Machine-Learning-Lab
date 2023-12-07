#include <SorterInterface.h>
#include <GraphEngineInterface.h>

class DefaultSorter : public SorterInterface {
public:
	ProcessingOrder SortGraph(GraphInterface* graph,bool DirectionForward) override {
		ProcessingOrder order;
		std::map<unsigned int, NodeInterface*> nodes = graph->GetNodes();
		std::map<unsigned int,EdgeInterface*> Edges=graph->GetEdges();
		std::vector<std::vector<NodeInterface*>> NodeOrder;
		std::vector<std::vector<EdgeInterface*>> EdgeOrder;

		if (DirectionForward) {
			//first node layer
			std::vector<NodeInterface*> FirstLayer;
			for (int i = 0; i < nodes.size(); i++)
			{
				if (nodes[i]->GetInputEdges().size() == 0) {
					FirstLayer.push_back(nodes[i]);
					nodes[i]->Visit();
				}
			}
			NodeOrder.push_back(FirstLayer);

			while (true) {
				std::vector<NodeInterface*> PrevLayer = NodeOrder.back();
				std::vector<NodeInterface*> NextLayer;
				std::vector<EdgeInterface*> NextLayerEdges;
				for (int i = 0; i < PrevLayer.size(); i++) {
					std::vector<EdgeInterface*> Edges = PrevLayer[i]->GetOutputEdges();
					for (int j = 0; j < Edges.size(); j++) {
						NextLayerEdges.push_back(Edges[j]);
						if (std::find(NextLayer.begin(), NextLayer.end(), Edges[j]->GetSecond()) == NextLayer.end()) {
							std::vector<EdgeInterface*> inEdges = Edges[j]->GetSecond()->GetInputEdges();
							bool HasUnvisitedInputs = false;
							for (int l = 0; l < inEdges.size(); l++)
							{
								if (!inEdges[l]->GetFirst()->IsVisited()) {
									HasUnvisitedInputs = true;
								}
							}

							if (!HasUnvisitedInputs) {
								if (Edges[j]->GetSecond()->IsVisited() == false) {
									NextLayer.push_back(Edges[j]->GetSecond());
								}
							}
						}
					}
				}
				if (NextLayerEdges.size() == 0) {
					break;
				}
				for (int i = 0; i < NextLayer.size(); i++) {
					NextLayer[i]->Visit();
				}
				EdgeOrder.push_back(NextLayerEdges);
				if (NextLayer.size() == 0) {
					break;
				}
				NodeOrder.push_back(NextLayer);
			}
			for (int i = 0; i < nodes.size(); i++)
			{
				nodes[i]->Unvisit();
			}

			order.Nodes = NodeOrder;
			order.Edges = EdgeOrder;

			return order;
		}
		else {
			//first node layer
			std::vector<NodeInterface*> FirstLayer;
			for (int i = 0; i < nodes.size(); i++)
			{
				if (nodes[i]->GetOutputEdges().size() == 0) {
					FirstLayer.push_back(nodes[i]);
					nodes[i]->Visit();
				}
			}
			NodeOrder.push_back(FirstLayer);

			while (true) {
				std::vector<NodeInterface*> PrevLayer = NodeOrder.back();
				std::vector<NodeInterface*> NextLayer;
				std::vector<EdgeInterface*> NextLayerEdges;
				for (int i = 0; i < PrevLayer.size(); i++) {
					std::vector<EdgeInterface*> Edges = PrevLayer[i]->GetInputEdges();
					for (int j = 0; j < Edges.size(); j++) {
						NextLayerEdges.push_back(Edges[j]);
						if (std::find(NextLayer.begin(), NextLayer.end(), Edges[j]->GetFirst()) == NextLayer.end()) {
							std::vector<EdgeInterface*> inEdges = Edges[j]->GetFirst()->GetOutputEdges();
							bool HasUnvisitedInputs = false;
							for (int l = 0; l < inEdges.size(); l++)
							{
								if (!inEdges[l]->GetSecond()->IsVisited()) {
									HasUnvisitedInputs = true;
								}
							}

							if (!HasUnvisitedInputs) {
								if (Edges[j]->GetFirst()->IsVisited() == false) {
									NextLayer.push_back(Edges[j]->GetFirst());
								}
							}
						}
					}
				}
				if (NextLayerEdges.size() == 0) {
					break;
				}
				for (int i = 0; i < NextLayer.size(); i++) {
					NextLayer[i]->Visit();
				}
				EdgeOrder.push_back(NextLayerEdges);
				if (NextLayer.size() == 0) {
					break;
				}
				NodeOrder.push_back(NextLayer);
			}
			for (int i = 0; i < nodes.size(); i++)
			{
				nodes[i]->Unvisit();
			}

			order.Nodes = NodeOrder;
			order.Edges = EdgeOrder;

			return order;
		}
	}
	DefaultSorter() {
		Name = "DefaultSorter";
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) SorterInterface* GetInstance() {
		return new DefaultSorter();
	}
}
