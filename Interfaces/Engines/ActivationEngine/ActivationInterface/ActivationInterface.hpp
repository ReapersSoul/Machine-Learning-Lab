#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace NS_Activation {

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

	static std::unordered_map<std::string, ActivationInterface*> Activations;
	static void RegisterActivation(std::string Name, ActivationInterface* Activation) {
		if (Activations.find(Name) != Activations.end()) throw std::runtime_error("Activation already registered");
		Activations[Name] = Activation;
	}
}