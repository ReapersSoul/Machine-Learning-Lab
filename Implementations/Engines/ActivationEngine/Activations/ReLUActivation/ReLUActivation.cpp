#include "../../../../Util/Exports.hpp"
#include <ActivationInterface.hpp>
#include <math.h>

class ReLUActivation : public NS_Activation::ActivationInterface
{
public:
    double Activate(double input) {
        return input > 0 ? input : 0;
    }

    double ActivateDerivative(double input) {
        return input > 0 ? 1 : 0;
    }

    ReLUActivation() {
        Name = "ReLU";
    }
};



extern "C" {
    // Define a function that returns the result of adding two numbers
    EXPORT NS_Activation::ActivationInterface* GetInstance() {
        return new ReLUActivation();
    }
}