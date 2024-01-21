#include "../../../../Util/Exports.hpp"
#include <LossInterface.hpp>
#include <math.h>

class MSELoss : public NS_Loss::LossInterface
{
public:
    double CalculateLoss(double input, double target) override{
        return pow(input - target, 2);
    }

    double CalculateLossDerivative(double input, double target) override {
        return 2 * (input - target);
    }

    MSELoss() {
        Name = "MSE";
    }
};

extern "C" {
    // Define a function that returns the result of adding two numbers
    EXPORT NS_Loss::LossInterface* GetInstance() {
        return new MSELoss();
    }
}