#pragma once
#include "../Graph/Graph.hpp"

class OptimizerInterface
{
protected:
	std::string Name;
	enum Type
	{
		ForwardGradient,
		Fitness,
	}type;
public:

	OptimizerInterface() {
		Name = "Optimizer";
	}

	void SetName(std::string name) {
		Name = name;
	}

	std::string GetName() {
		return Name;
	}

	void SetType(Type type) {
		this->type = type;
	}

	Type GetType() {
		return type;
	}

	virtual void Optimize(Graph* graph) = 0;
	virtual double DetermineFitness(Graph* graph) = 0;
	virtual void ProvideForwardGradient(Graph* graph) = 0;
};