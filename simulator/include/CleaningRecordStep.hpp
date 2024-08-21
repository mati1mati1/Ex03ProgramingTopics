#pragma once
#include "enums.h"
#include "HouseLocation.hpp"

class CleaningRecordStep{
    public:
        CleaningRecordStep(LocationType locationType,Step step , uint32_t batteryLevel, uint32_t dirtLevel) :
         locationType(locationType), step(step),batteryLevel(batteryLevel),dirtLevel(dirtLevel) { };
        const LocationType& getLocationType() const { return locationType; }
        const Step& getStep() const { return step; }
        uint32_t getBatteryLevel() const { return batteryLevel; }
        bool isAtDockingStation() const { return getLocationType() == LocationType::CHARGING_STATION; } 
        uint32_t getDirtLevel() const { return dirtLevel; }
        bool operator==(const CleaningRecordStep& other) const
        {
            return locationType == other.locationType && step == other.step &&
                batteryLevel == other.batteryLevel && dirtLevel == other.dirtLevel;
        }
        friend std::ostream& operator<<(std::ostream& os, const CleaningRecordStep& record);
    private:
        const LocationType locationType;
        const Step step;
        const uint32_t batteryLevel;
        const uint32_t dirtLevel;
};