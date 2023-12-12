#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace NS_Loss {

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
	
	
	static std::unordered_map<std::string, LossInterface*> Losses;
	static void RegisterLoss(std::string Name, LossInterface* Loss) {
		if (Losses.find(Name) != Losses.end()) throw std::runtime_error("Loss already registered");
		Losses[Name] = Loss;
	}
}