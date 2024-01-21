#include "../../../../Util/Exports.hpp"
#include <SorterInterface.hpp>
#include <GraphEngineInterface.hpp>

class DefaultSorter : public NS_Sorter::SorterInterface {
public:
	NS_Sorter::ProcessingOrder SortGraph(Graph* graph,bool DirectionForward) override {
		NS_Sorter::ProcessingOrder order;
		std::map<unsigned int, NS_Node::NodeInterface*> nodes = graph->GetNodes();
		std::map<unsigned int,Edge> Edges=graph->GetEdges();
		std::vector<std::vector<NS_Node::NodeInterface*>> NodeOrder;
		std::vector<std::vector<Edge*>> EdgeOrder;

		if (DirectionForward) {
			//first node layer
			std::vector<NS_Node::NodeInterface*> FirstLayer;
			for (int i = 0; i < nodes.size(); i++)
			{
				if (nodes[i]->GetInputEdges().size() == 0) {
					FirstLayer.push_back(nodes[i]);
					nodes[i]->Visit();
				}
			}
			NodeOrder.push_back(FirstLayer);

			while (true) {
				std::vector<NS_Node::NodeInterface*> PrevLayer = NodeOrder.back();
				std::vector<NS_Node::NodeInterface*> NextLayer;
				std::vector<Edge*> NextLayerEdges;
				for (int i = 0; i < PrevLayer.size(); i++) {
					std::vector<Edge*> Edges = PrevLayer[i]->GetOutputEdges();
					for (int j = 0; j < Edges.size(); j++) {
						NextLayerEdges.push_back(Edges[j]);
						std::vector<NS_Node::NodeInterface*>::iterator it = std::find_if(NextLayer.begin(), NextLayer.end(), [&](NS_Node::NodeInterface* node) {return node->GetUID() == Edges[j]->GetSecond(); });
						if (it == NextLayer.end()) {
							std::vector<Edge*> inEdges = graph->GetNodes()[Edges[j]->GetSecond()]->GetInputEdges();
							bool HasUnvisitedInputs = false;
							for (int l = 0; l < inEdges.size(); l++)
							{
								if (!graph->GetNodes()[inEdges[l]->GetFirst()]->IsVisited()) {
									HasUnvisitedInputs = true;
								}
							}

							if (!HasUnvisitedInputs) {
								if (graph->GetNodes()[Edges[j]->GetSecond()]->IsVisited() == false) {
									NextLayer.push_back(graph->GetNodes()[Edges[j]->GetSecond()]);
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

			return order;
		}
		else {
			//first node layer
			std::vector<NS_Node::NodeInterface*> FirstLayer;
			for (int i = 0; i < nodes.size(); i++)
			{
				if (nodes[i]->GetOutputEdges().size() == 0) {
					FirstLayer.push_back(nodes[i]);
					nodes[i]->Visit();
				}
			}
			NodeOrder.push_back(FirstLayer);

			while (true) {
				std::vector<NS_Node::NodeInterface*> PrevLayer = NodeOrder.back();
				std::vector<NS_Node::NodeInterface*> NextLayer;
				std::vector<Edge*> NextLayerEdges;
				for (int i = 0; i < PrevLayer.size(); i++) {
					std::vector<Edge*> Edges = PrevLayer[i]->GetInputEdges();
					for (int j = 0; j < Edges.size(); j++) {
						NextLayerEdges.push_back(Edges[j]);
						std::vector<NS_Node::NodeInterface*>::iterator it = std::find_if(NextLayer.begin(), NextLayer.end(), [&](NS_Node::NodeInterface* node) {return node->GetUID() == Edges[j]->GetFirst(); });
						if (it == NextLayer.end()) {
							std::vector<Edge*> inEdges = graph->GetNodes()[Edges[j]->GetFirst()]->GetOutputEdges();
							bool HasUnvisitedInputs = false;
							for (int l = 0; l < inEdges.size(); l++)
							{
								if (!graph->GetNodes()[inEdges[l]->GetSecond()]->IsVisited()) {
									HasUnvisitedInputs = true;
								}
							}

							if (!HasUnvisitedInputs) {
								if (graph->GetNodes()[Edges[j]->GetFirst()]->IsVisited() == false) {
									NextLayer.push_back(graph->GetNodes()[Edges[j]->GetFirst()]);
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

			return order;
		}
	}
	DefaultSorter() {
		Name = "DefaultSorter";
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT NS_Sorter::SorterInterface* GetInstance() {
		return new DefaultSorter();
	}
}
