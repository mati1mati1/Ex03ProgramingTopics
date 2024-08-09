#pragma once
#include "Direction.hpp"
#include "Step.hpp"
#include <iostream>
#include <cmath>

template <typename TCoordinateType>
class Coordinate {
public:
    Coordinate() : x(0), y(0) {}
    Coordinate(TCoordinateType x, TCoordinateType y) : x(x), y(y) {}
    
    TCoordinateType getX() const { return x; }
    TCoordinateType getY() const { return y; }

    Coordinate getDirection(const Direction& direction) const;
    Coordinate getStep(const Step& step) const;
    double distance(const Coordinate& other = Coordinate(0,0)) const {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }
    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Coordinate& other) const {
        return !(*this == other);
    }
    bool operator<(const Coordinate& other) const {
        if (x == other.x) {
            return y < other.y;
        }
        return x < other.x;
    }
    Direction getDirection(const Coordinate<TCoordinateType>& to) const{
        if (getX() == to.getX()) {
            if (getY() < to.getY()) {
                return Direction::East;
            } else {
                return Direction::West;
            }
        } else {
            if (getX() < to.getX()) {
                return Direction::South;
            } else {
                return Direction::North;
            }
        }
    }
    template <typename T>
    friend std::ostream& operator<<(std::ostream& os, const Coordinate<T>& coord);

private:
    TCoordinateType x;
    TCoordinateType y;
};
template <typename TCoordinateType>
inline std::ostream& operator<<(std::ostream& os, const Coordinate<TCoordinateType>& coord) {
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
}
namespace std {
    template <typename TCoordinateType>
    struct hash<Coordinate<TCoordinateType>> {
        std::size_t operator()(const Coordinate<TCoordinateType>& coordinate) const {
            auto h1 = std::hash<TCoordinateType>{}(coordinate.getX());
            auto h2 = std::hash<TCoordinateType>{}(coordinate.getY());
            return h1 ^ (h2 << 1); 
        }
    };
}
