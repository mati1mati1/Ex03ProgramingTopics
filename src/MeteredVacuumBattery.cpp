#include "MeteredVacuumBattery.hpp"
#include "Logger.hpp"
double MeteredVacuumBattery::calculateNewBatteryLevel(uint32_t steps) {
    Logger::getInstance().log("Calculating new battery level with steps: " + std::to_string(steps));
    // Without the compensation a 0.999999999997 charge level as a result of double inherit inaccuracy would be considered as 0
    double newBatteryLevel = this->batteryLevel + steps * ((double)maxBatterySteps / 20);
    Logger::getInstance().log("New battery level calculated: " + std::to_string(newBatteryLevel));
    return newBatteryLevel;
}

void MeteredVacuumBattery::charge(uint32_t steps) {
    Logger::getInstance().log("Charging vacuum with steps: " + std::to_string(steps));
    double newBatteryLevel = calculateNewBatteryLevel(steps);
    this->batteryLevel = newBatteryLevel;
    if (newBatteryLevel > maxBatterySteps) {
        this->batteryLevel = maxBatterySteps;
        Logger::getInstance().log("Battery level exceeds max battery steps. Setting to max: " + std::to_string(maxBatterySteps));
    }
    Logger::getInstance().log("VacuumSimulator charged. New battery level: " + std::to_string(this->batteryLevel));
}

bool MeteredVacuumBattery::try_activate(uint32_t steps) {
    Logger::getInstance().log("Trying to activate vacuum with steps: " + std::to_string(steps));
    double newBatteryLevel = getCompensatedBatteryLevel() - steps;
    if (newBatteryLevel < 0) {
        this->batteryLevel = 0;
        Logger::getInstance().log("Not enough battery. Activation failed. Battery level set to 0.");
        return false;
    }
    this->batteryLevel = newBatteryLevel;
    Logger::getInstance().log("VacuumSimulator activated successfully. New battery level: " + std::to_string(this->batteryLevel));
    return true;
}
