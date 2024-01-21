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

	class Registrar {
		std::unordered_map<std::string, ActivationInterface*> Activations;
	public:
		Registrar();
		std::unordered_map<std::string, ActivationInterface*> &GetActivations();
		void RegisterActivation(std::string Name, ActivationInterface* Activation);
		ActivationInterface* GetActivation(std::string Name);
	};
}