#pragma once
#include "House.hpp"
#include "Direction.hpp"
class PlacedHouse : public House
{
    public: 
        virtual HouseLocation& getCurrentLocation() = 0;
        virtual HouseLocation& getDirectionLocation(const Direction& direction) = 0;
        virtual bool is_move(const Direction& direction) = 0;
        virtual void move(const Direction& direction) = 0;
        virtual ~PlacedHouse() {};
};