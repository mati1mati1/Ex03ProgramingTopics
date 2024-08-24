#include "AlgorithmRegistration.h"

#include <stdexcept>
#include <thread>

class timingOutSometimesFaultySometimes : public AbstractAlgorithm
{
public:
    ~timingOutSometimesFaultySometimes() override = default;
     void setMaxSteps(std::size_t maxSteps) override { 
        this->maxSteps = maxSteps; 
        whenToFault = maxSteps / 2;
        shouldFault = std::rand() % 2 == 0;
        shouldTimeout = std::rand() % 2 == 0;
    };
    
    void setWallsSensor(const WallsSensor &) override { };
    void setDirtSensor(const DirtSensor &) override { };
    void setBatteryMeter(const BatteryMeter &) override{ };
    Step nextStep() override { 

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
                if (shouldFault && maxSteps <= whenToFault)
                {
                    throw std::runtime_error("Faulty algorithm");
                }
                maxSteps--;
                return Step::Stay;
            }
            return Step::Finish;
        }
        return Step::Finish;
    }
    private:
    std::size_t maxSteps;
    bool shouldFault ;
    size_t whenToFault;
    bool shouldTimeout;
};
REGISTER_ALGORITHM(timingOutSometimesFaultySometimes);