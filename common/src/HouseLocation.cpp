#include "HouseLocation.hpp"
#include <stdexcept>

HouseLocation::HouseLocation(LocationType locationType, uint8_t dirtLevel) {
    this->type = locationType;
    if (locationType != LocationType::HOUSE_TILE) {
        this->dirtLevel = 0;
        return;
    }
    if (!isValidDirtLevel(dirtLevel)) {
        dirtLevel = 0;
    }
    this->dirtLevel = dirtLevel;
}
std::ostream& operator<<(std::ostream& os, const HouseLocation& house)
{
    switch (house.getLocationType()) {
    case LocationType::HOUSE_TILE:
        os << std::to_string(house.getDirtLevel());
        break;
    case LocationType::WALL:
        os << 'W';
        break;
    case LocationType::CHARGING_STATION:
        os << 'X';
        break;
    default:
        break;
    }
    return os;
}
HouseLocation::HouseLocation(char locationEncoding) {
    switch (locationEncoding) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        this->type = LocationType::HOUSE_TILE;
        this->dirtLevel = locationEncoding - '0';
        break;
    case ' ':
        this->type = LocationType::HOUSE_TILE;
        this->dirtLevel = 0;
        break;
    case 'W':
        this->type = LocationType::WALL;
        break;
    case 'D':
        this->type = LocationType::CHARGING_STATION;
        break;
    default:
        this->type = LocationType::HOUSE_TILE;
        this->dirtLevel = 0;
        break;
    }
}

void HouseLocation::setDirtLevel(uint8_t dirtLevel) {
    if (this->type != LocationType::HOUSE_TILE) {
        throw std::runtime_error("Only house tile can have dirt level");
    }
    if (!isValidDirtLevel(dirtLevel)) {
        throw std::runtime_error("House tile must have valid dirt level");
    }
    this->dirtLevel = dirtLevel;
}

bool HouseLocation::isValidDirtLevel(uint8_t dirtLevel) {
    bool isValid = dirtLevel <= 9;
    return isValid;
}

std::ostream& operator<<(std::ostream& os, const LocationType& locationType) {
    switch (locationType) {
        case LocationType::CHARGING_STATION:
            os << "CHARGING_STATION";
            break;
        case LocationType::WALL:
            os << "WALL";
            break;
        case LocationType::HOUSE_TILE:
            os << "HOUSE_TILE";
            break;
        default:
            os << "CORRIDOR";
            break;
    }
    return os;
}