#pragma once
#include "../../Nodes/NodeInterface/NodeInterface.h"
#include <vector>

class TrainingGroupInterface {
	GraphInterface* Graph;
	std::vector<NodeInterface*> Nodes;
public:
		TrainingGroupInterface() {
			TypeID = "TrainingGroup";
		}

};