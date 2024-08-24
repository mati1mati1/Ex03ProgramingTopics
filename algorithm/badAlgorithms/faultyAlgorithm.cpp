#include "AlgorithmRegistration.h"

class faultyAlgorithm : public AbstractAlgorithm
{
public:
    ~faultyAlgorithm() override = default;
    void setMaxSteps(std::size_t maxSteps) override { this->maxSteps = maxSteps; };
    void setWallsSensor(const WallsSensor &) override { };
    void setDirtSensor(const DirtSensor &) override { };
    void setBatteryMeter(const BatteryMeter &) override{ };
    Step nextStep() override { 
        throw std::runtime_error("Didn't implement/failed"); 
    }
    private:
    std::size_t maxSteps;
};
REGISTER_ALGORITHM(faultyAlgorithm);
