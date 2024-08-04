#pragma once
#include <HouseLocation.hpp>
#include <Coordinate.hpp>
class HouseLocationMapping
{
    public:
        HouseLocationMapping(const Coordinate<int32_t>& relativeCoordinates, const HouseLocation& houseLocation): relativeCoordinates(relativeCoordinates), houseLocation(houseLocation){};
        [[nodiscard]] Coordinate<int32_t> getRelativeToCharger() const { return relativeCoordinates; }
        [[nodiscard]] HouseLocation& getHouseLocation() { return houseLocation; }
        [[nodiscard]] const HouseLocation& getHouseLocation() const { return houseLocation; }
        void update(const HouseLocation& newlocation) {
            this->houseLocation = newlocation; 
        }
    private:
        Coordinate<int32_t> relativeCoordinates;
        HouseLocation houseLocation;
};
