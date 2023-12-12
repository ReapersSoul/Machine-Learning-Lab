#include "../../../../Util/Exports.hpp"
#include <ActivationInterface.hpp>
#include <math.h>

class SigmoidActivation : public NS_Activation::ActivationInterface
{
public:
    double Activate(double input) {
        return 1 / (1 + exp(-input));
    }

    double ActivateDerivative(double input) {
		return Activate(input) * (1 - Activate(input));
	}

    SigmoidActivation() {
		Name = "Sigmoid";
	}
};



extern "C" {
    // Define a function that returns the result of adding two numbers
    EXPORT NS_Activation::ActivationInterface* GetInstance() {
        return new SigmoidActivation();
    }
}