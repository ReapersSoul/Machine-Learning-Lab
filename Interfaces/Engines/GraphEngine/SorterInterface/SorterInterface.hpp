#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace NS_Node {
	class NodeInterface;
}
class Graph;

namespace NS_Sorter {
	struct ProcessingOrder {
		std::vector<std::vector<NS_Node::NodeInterface*>> Nodes;
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

	static std::unordered_map<std::string, SorterInterface*> Sorters;
	static void RegisterSorter(std::string Name, SorterInterface* Sorter) {
		if (Sorters.find(Name) != Sorters.end()) throw std::runtime_error("Sorter already registered");
		Sorters[Name] = Sorter;
	}
}