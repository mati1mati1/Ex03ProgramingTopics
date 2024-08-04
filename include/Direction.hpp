#pragma once
#include <iostream>
#include "enums.h"
inline std::ostream& operator<<(std::ostream& os, const Direction& direction) {
    switch (direction) {
        case Direction::North: os << "North"; break;
        case Direction::East: os << "East"; break;
        case Direction::South: os << "South"; break;
        case Direction::West: os << "West"; break;
    }
    return os;
}
namespace DirectionTools
{
    inline Step toStep(Direction step) {
        switch (step) {
            case Direction::North: return Step::North;
            case Direction::East: return Step::East;
            case Direction::South: return Step::South;
            case Direction::West: return Step::West;
            default: return Step::Stay; // Handle any unexpected case
        }
    }
    inline Direction reduceStepToDirection(Step step) {
        switch (step) {
            case Step::North: return Direction::North;
            case Step::East: return Direction::East;
            case Step::South: return Direction::South;
            case Step::West: return Direction::West;
            default: throw std::runtime_error("Invalid reduce"); // Handle any unexpected case
        }
    }
    inline bool isStayInPlaceStep(Step step) {
        return step == Step::Stay || step == Step::Finish; 
    }
};
inline Direction operator~(Direction dir) {
    switch (dir) {
        case Direction::North: return Direction::South;
        case Direction::East: return Direction::West;
        case Direction::South: return Direction::North;
        case Direction::West: return Direction::East;
    }
    std::cerr << "Invalid direction" << std::endl;
    throw std::runtime_error("Invalid direction");
}