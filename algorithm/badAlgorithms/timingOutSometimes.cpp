#include "AlgorithmRegistration.h"

#include <stdexcept>
#include <thread>

class timingOutSometimes : public AbstractAlgorithm
{
public:
    ~timingOutSometimes() override = default;
    void setMaxSteps(std::size_t maxSteps) override { this->maxSteps = maxSteps; };
    void setWallsSensor(const WallsSensor &) override { };
    void setDirtSensor(const DirtSensor &) override { };
    void setBatteryMeter(const BatteryMeter &) override{ };
    Step nextStep() override { 
        
        bool shouldTimeout = std::rand() % 2 == 0;
        if(shouldTimeout)
        {
            while(maxSteps > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                maxSteps--;
                return Step::Stay;
            }
        }
        else
        {
            if (maxSteps != 0)
            {
                maxSteps--;
                return Step::Stay;
            }
            return Step::Finish;
        }
        return Step::Finish;
    }
    private:
    std::size_t maxSteps;
};
REGISTER_ALGORITHM(timingOutSometimes);