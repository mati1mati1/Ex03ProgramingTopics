#pragma once
#include "PlacedHouse.hpp"
#include "Coordinate.hpp"
#include "DirtSensor.h"
#include "WallSensor.h"
#include "BatteryMeter.h"
#include "enums.h"
class VacuumHouse : public PlacedHouse , public DirtSensor, public WallsSensor
{
    public:
        VacuumHouse(const std::vector<std::string>& locations,uint32_t rows,uint32_t cols);
        ~VacuumHouse() override { };
        uint32_t getTotalDirt() const override;
        HouseLocation &getCurrentLocation() override;
        const HouseLocation &getCurrentLocation() const;
        HouseLocation &getDirectionLocation(const Direction &direction) override;
        const HouseLocation &getDirectionLocation(const Direction &direction) const;
        HouseLocation &getStepLocation(const Step &step);
        void cleanCurrentLocation();
        bool is_move(const Step & step);
        bool is_move(const Direction &direction) override;
        void move(const Step &direction);
        void move(const Direction &direction) override;


        bool isWall(Direction d) const override { return getDirectionLocation(d).getLocationType() == LocationType::WALL; };
        int dirtLevel() const override { return getCurrentLocation().getDirtLevel(); };

        friend std::ostream &operator<<(std::ostream &os, const VacuumHouse &house);
    private:
        Coordinate<size_t> currentLocation;
        std::vector<std::vector<HouseLocation>> houseLocations;
        HouseLocation logicalWall;
        const HouseLocation* chargingStation;

    private:
        std::vector<std::vector<HouseLocation>> constructHouseLocation(const std::vector<std::string> &locations, uint32_t rows, uint32_t cols);
        bool inBounds(Coordinate<size_t> &coordinate) const;
};