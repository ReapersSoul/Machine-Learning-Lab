#include <ActivationInterface.h>
#include <math.h>

class ReLUActivation : public ActivationInterface
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
    __declspec(dllexport) ActivationInterface* GetInstance() {
        return new ReLUActivation();
    }
}