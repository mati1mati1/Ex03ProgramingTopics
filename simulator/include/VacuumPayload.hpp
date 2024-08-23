#pragma once
#include "VacuumHouse.hpp"
#include "MeteredVacuumBattery.hpp"
#include <chrono>
class VacuumPayload {
    public:
        VacuumPayload(VacuumHouse house ,MeteredVacuumBattery battery,uint32_t maxSteps):
            house(house),battery(battery),maxSteps(maxSteps) {};
        VacuumHouse& getHouse() {return house;}
        MeteredVacuumBattery& getBattery() {return battery;}
        uint32_t getMaxSteps() const {return maxSteps;}
        auto getMaxTime() const { return std::chrono::milliseconds(maxSteps) * 5 + std::chrono::milliseconds(100); }

    private:
        VacuumHouse house;
        MeteredVacuumBattery battery;
        uint32_t maxSteps;
};