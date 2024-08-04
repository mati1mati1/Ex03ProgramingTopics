#pragma once
#include "VacuumHouse.hpp"
#include "MeteredVacuumBattery.hpp"
class VacuumPayload {
    public:
        VacuumPayload(VacuumHouse house ,MeteredVacuumBattery battery,uint32_t maxSteps):
            house(house),battery(battery),maxSteps(maxSteps) {};
        VacuumHouse& getHouse() {return house;}
        MeteredVacuumBattery& getBattery() {return battery;}
        uint32_t getMaxSteps() const {return maxSteps;}

    private:
        VacuumHouse house;
        MeteredVacuumBattery battery;
        uint32_t maxSteps;
};