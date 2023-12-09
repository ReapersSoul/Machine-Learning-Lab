#pragma once
#include <string>

class ActivationInterface {
protected:
	std::string Name;

public:
	virtual std::string GetName() {
			return Name;
	}
	virtual double Activate(double input) = 0;
	virtual double ActivateDerivative(double input) = 0;
};