#pragma once
#include <string>
#include <vector>

namespace NodeInterface {
	class NodeInterface;
}
class Graph;

struct ProcessingOrder {
	std::vector<std::vector<NodeInterface::NodeInterface*>> Nodes;
};

class SorterInterface {
protected:
	std::string Name;
public:
	virtual ProcessingOrder SortGraph(Graph* Graph, bool DirectionForward) = 0;
	virtual std::string GetName() {
		return Name;
	}
};