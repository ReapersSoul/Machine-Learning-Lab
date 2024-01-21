#include "../../../../Util/Exports.hpp"
#include <ActivationInterface.hpp>
#include <math.h>

class LeakyReLUActivation : public NS_Activation::ActivationInterface
{
public:
    double Activate(double input) {
        return input > 0 ? input : 0.01 * input;
    }

    double ActivateDerivative(double input) {
        return input > 0 ? 1 : 0.01;
    }

    LeakyReLUActivation() {
        Name = "LeakyReLU";
    }
};



extern "C" {
    // Define a function that returns the result of adding two numbers
    EXPORT NS_Activation::ActivationInterface* GetInstance() {
        return new LeakyReLUActivation();
    }

    EXPORT void Register(void* Registrar) {
        NS_Activation::Registrar* registrar = (NS_Activation::Registrar*) Registrar;
        registrar->RegisterActivation("LeakyReLU", new LeakyReLUActivation());
    }
}