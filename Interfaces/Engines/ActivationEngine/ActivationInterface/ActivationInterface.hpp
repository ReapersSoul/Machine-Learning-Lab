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
		static std::unordered_map<std::string, ActivationInterface*> Activations;
		Registrar();
	public:
		static Registrar* GetRegistrar();

		static std::unordered_map<std::string, ActivationInterface*> &GetActivations();
		static void RegisterActivation(std::string Name, ActivationInterface* Activation);
		static ActivationInterface* GetActivation(std::string Name);
	};

	static Registrar* GetRegistrar(){
		return Registrar::GetRegistrar();
	}
}