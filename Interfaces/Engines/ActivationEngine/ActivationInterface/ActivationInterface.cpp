#include "ActivationInterface.hpp"

namespace NS_Activation
{

	std::unordered_map<std::string, ActivationInterface *> Registrar::Activations = std::unordered_map<std::string, ActivationInterface *>();

	Registrar::Registrar()
	{
		Activations = {};
	}

	Registrar *Registrar::GetRegistrar()
	{
		static Registrar registrar;
		return &registrar;
	}

	Registrar *GetRegistrar()
	{
		return Registrar::GetRegistrar();
	}
}