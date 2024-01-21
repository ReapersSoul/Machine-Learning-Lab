#include "LossInterface.hpp"

namespace NS_Loss
{
	Registrar::Registrar()
	{
		Losses = {};
	}

	std::unordered_map<std::string, LossInterface *> &Registrar::GetLosses()
	{
		return Losses;
	}

	void Registrar::RegisterLoss(std::string Name, LossInterface *Loss)
	{
		Losses[Name] = Loss;
	}

	LossInterface *Registrar::GetLoss(std::string Name)
	{
		if (Losses.find(Name) == Losses.end())
			throw std::runtime_error("Loss not registered");
		return Losses[Name];
	}
}