#pragma once
#include <string>
#include <vector>

class NodeInterface;
class EdgeInterface;
class GraphInterface;

struct ProcessingOrder {
	std::vector<std::vector<NodeInterface*>> Nodes;
	std::vector<std::vector<EdgeInterface*>> Edges;
};

class SorterInterface {
protected:
	std::string Name;
public:
	virtual ProcessingOrder SortGraph(GraphInterface* Graph, bool DirectionForward) = 0;
	virtual std::string GetName() {
		return Name;
	}
};