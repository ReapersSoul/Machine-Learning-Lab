#pragma once
#include "../../Nodes/NodeInterface/NodeInterface.h"

class SubGraphNodeInterface : public NodeInterface {
	GraphInterface* Graph;
public:
		SubGraphNodeInterface() {
			TypeID = "SubGraphNode";
		}
};