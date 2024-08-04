#include "Coordinate.hpp"
#include "Logger.hpp"

template <typename TCoordinateType>
Coordinate<TCoordinateType> Coordinate<TCoordinateType>::getDirection(const Direction& direction) const
{
    switch (direction)
    {
        case Direction::North:
            return Coordinate(this->getX()-1,this->getY());
        case Direction::South:
            return Coordinate(this->getX()+1,this->getY());
        case Direction::East:
            return Coordinate(this->getX(),this->getY()+1);
        case Direction::West:
            return Coordinate(this->getX(),this->getY()-1);
    }
    Logger::getInstance().log("Unknown direction fetched");
    return Coordinate(this->getX(),this->getY());
}
template <typename TCoordinateType>
Coordinate<TCoordinateType> Coordinate<TCoordinateType>::getStep(const Step& step) const
{
    switch (step)
    {
        case Step::North:
            return Coordinate(this->getX()-1,this->getY());
        case Step::South:
            return Coordinate(this->getX()+1,this->getY());
        case Step::East:
            return Coordinate(this->getX(),this->getY()+1);
        case Step::West:
            return Coordinate(this->getX(),this->getY()-1);
        case Step::Finish:
        case Step::Stay:
            return Coordinate(this->getX(),this->getY());
    }
    Logger::getInstance().log("Unknown step fetched");
    return Coordinate(this->getX(),this->getY());
}

template class Coordinate<size_t>;
template class Coordinate<int32_t>;