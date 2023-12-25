#include "ActivationInterface.hpp"

namespace NS_Activation
{

	Registrar::Registrar()
	{
		Activations = {};
	}

	std::unordered_map<std::string, ActivationInterface *> &Registrar::GetActivations()
	{
		return Activations;
	}

	void Registrar::RegisterActivation(std::string Name, ActivationInterface *Activation)
	{
		if (Activations.find(Name) != Activations.end())
			throw std::runtime_error("Activation already registered");
		Activations[Name] = Activation;
	}

	ActivationInterface *Registrar::GetActivation(std::string Name)
	{
		if (Activations.find(Name) == Activations.end())
			throw std::runtime_error("Activation not registered");
		return Activations[Name];
	}
}