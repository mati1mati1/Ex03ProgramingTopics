#include "MeteredVacuumBattery.hpp"
double MeteredVacuumBattery::calculateNewBatteryLevel(uint32_t steps) {
    // Without the compensation a 0.999999999997 charge level as a result of double inherit inaccuracy would be considered as 0
    double newBatteryLevel = this->batteryLevel + steps * ((double)maxBatterySteps / 20);
    return newBatteryLevel;
}

void MeteredVacuumBattery::charge(uint32_t steps) {
    double newBatteryLevel = calculateNewBatteryLevel(steps);
    this->batteryLevel = newBatteryLevel;
    if (newBatteryLevel > maxBatterySteps) {
        this->batteryLevel = maxBatterySteps;
    }
}

bool MeteredVacuumBattery::try_activate(uint32_t steps) {
    double newBatteryLevel = getCompensatedBatteryLevel() - steps;
    if (newBatteryLevel < 0) {
        this->batteryLevel = 0;
        return false;
    }
    this->batteryLevel = newBatteryLevel;
    return true;
}
