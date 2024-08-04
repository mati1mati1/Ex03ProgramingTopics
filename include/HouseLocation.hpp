#pragma once
#include <cstdint>
#include <optional>
#include <ostream>
enum class LocationType {
    WALL = 'W',
    CHARGING_STATION = 'D',
    HOUSE_TILE = 'T',
    UNKNOWN = '?'
};
std::ostream& operator<<(std::ostream& os, const LocationType& locationType);

class HouseLocation
{
    public:
        HouseLocation(LocationType locationType, uint8_t dirtLevel = 0 );
        HouseLocation(char locationEncoding);
        const LocationType& getLocationType() const { return type; };
        uint8_t getDirtLevel() const { return dirtLevel; };
        void setDirtLevel(uint8_t dirtLevel);
        friend std::ostream& operator<<(std::ostream& os, const HouseLocation& house);
        bool operator==(const HouseLocation& other) const { return &other == this; }
    private:
        LocationType type;
        uint8_t dirtLevel = 0;

        bool isValidDirtLevel(uint8_t dirtLevel);
};
