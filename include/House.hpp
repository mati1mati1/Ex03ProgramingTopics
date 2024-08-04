#pragma once
#include "HouseLocation.hpp"
#include <vector>
class House
{
    public:
        virtual uint32_t getTotalDirt() const = 0;
        virtual ~House() {};
};

