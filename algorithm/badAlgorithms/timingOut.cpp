#include "AlgorithmRegistration.h"

#include <stdexcept>
#include <thread>
class timingOut : public AbstractAlgorithm
{
public:
    ~timingOut() override = default;
    void setMaxSteps(std::size_t maxSteps) override { this->maxSteps = maxSteps; };
    void setWallsSensor(const WallsSensor &) override { };
    void setDirtSensor(const DirtSensor &) override { };
    void setBatteryMeter(const BatteryMeter &) override{ };
    Step nextStep() override { 

        while(maxSteps > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            maxSteps--;
            return Step::Stay;
        }
        return Step::Stay;
    }
    private:
    std::size_t maxSteps;
};
REGISTER_ALGORITHM(timingOut);