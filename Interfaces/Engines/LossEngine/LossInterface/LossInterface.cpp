#include "LossInterface.hpp"

namespace NS_Loss
{

	std::unordered_map<std::string, LossInterface *> Registrar::Losses = std::unordered_map<std::string, LossInterface *>();

	Registrar::Registrar()
	{
		Losses = {};
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