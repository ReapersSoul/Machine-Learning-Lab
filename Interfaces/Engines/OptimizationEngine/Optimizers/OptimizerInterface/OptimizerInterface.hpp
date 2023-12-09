#pragma once
#include "../GraphInterface/GraphInterface.hpp"

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

	virtual void Optimize(GraphInterface* graph) = 0;
	virtual double DetermineFitness(GraphInterface* graph) = 0;
	virtual void ProvideForwardGradient(GraphInterface* graph) = 0;
};