#pragma once
#include <string>

class LossInterface {
protected:
	std::string Name;

public:
	virtual std::string GetName() {
		return Name;
	}
	virtual double CalculateLoss(double input, double target) = 0;
	virtual double CalculateLossDerivative(double input, double target) = 0;
};