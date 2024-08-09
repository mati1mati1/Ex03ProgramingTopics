#pragma once
#include "BatteryMeter.h"
#include <cstdint>
class MeteredVacuumBattery : public BatteryMeter
{
    public:
        MeteredVacuumBattery(uint32_t batterySteps,uint32_t maxBatterySteps) : batteryLevel(batterySteps), maxBatterySteps(maxBatterySteps) {};
        void charge(uint32_t steps = 1);
        bool try_activate(uint32_t steps = 1);
        std::size_t getBatteryState() const override {return getCompensatedBatteryLevel();};
        uint32_t getMaxBatterySteps() {return maxBatterySteps;};
    private:
        constexpr static double DOUBLE_INACCURACY_COMPENSATION = 1e-9;
        double getCompensatedBatteryLevel() const {return batteryLevel + DOUBLE_INACCURACY_COMPENSATION;};
        double calculateNewBatteryLevel(uint32_t steps);
        double batteryLevel;
        uint32_t maxBatterySteps;
};