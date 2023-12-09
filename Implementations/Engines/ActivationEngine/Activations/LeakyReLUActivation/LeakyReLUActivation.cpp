#include "../../../../Util/Exports.hpp"
#include <ActivationInterface.hpp>
#include <math.h>

class LeakyReLUActivation : public ActivationInterface
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
    EXPORT ActivationInterface* GetInstance() {
        return new LeakyReLUActivation();
    }
}