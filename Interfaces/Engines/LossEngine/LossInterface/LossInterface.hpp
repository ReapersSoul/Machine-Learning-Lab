#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace NS_Loss
{

	class LossInterface
	{
	protected:
		std::string Name;

	public:
		virtual std::string GetName()
		{
			return Name;
		}
		virtual double CalculateLoss(double input, double target) = 0;
		virtual double CalculateLossDerivative(double input, double target) = 0;
	};

	class Registrar
	{
		std::unordered_map<std::string, LossInterface *> Losses;
	public:
		Registrar();
		std::unordered_map<std::string, LossInterface *> &GetLosses();
		void RegisterLoss(std::string Name, LossInterface *Loss);
		LossInterface *GetLoss(std::string Name);
	};
}