#include "HouseLocation.hpp"
#include "Logger.hpp" 
#include <stdexcept>

HouseLocation::HouseLocation(LocationType locationType, uint8_t dirtLevel) {
    Logger::getInstance().log("Initializing HouseLocation with LocationType: " + std::to_string(static_cast<int>(locationType)) + 
                              ", DirtLevel: " + (dirtLevel ? std::to_string(dirtLevel) : "none"));
    this->type = locationType;
    if (locationType != LocationType::HOUSE_TILE) {
        if (dirtLevel != 0) {
            Logger::getInstance().log("Warn: Only house tile can have dirt level ignoring");
        }
        this->dirtLevel = 0;
        return;
    }
    if (!isValidDirtLevel(dirtLevel)) {
        Logger::getInstance().log("Error: House tile must have valid dirt level, defaulting to 0");
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
        Logger::getInstance().log("Error: Invalid LocationType");
        break;
    }
    return os;
}
HouseLocation::HouseLocation(char locationEncoding) {
    Logger::getInstance().log("Initializing HouseLocation with encoding: " + std::string(1, locationEncoding));
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
        Logger::getInstance().log("Warn: Invalid char in house representation, defaulting to corridor");
        break;
    }
}

void HouseLocation::setDirtLevel(uint8_t dirtLevel) {
    Logger::getInstance().log("Setting dirt level to: " + std::to_string(dirtLevel));
    if (this->type != LocationType::HOUSE_TILE) {
        Logger::getInstance().log("Error: Only house tile can have dirt level");
        throw std::runtime_error("Only house tile can have dirt level");
    }
    if (!isValidDirtLevel(dirtLevel)) {
        Logger::getInstance().log("Error: House tile must have valid dirt level");
        throw std::runtime_error("House tile must have valid dirt level");
    }
    this->dirtLevel = dirtLevel;
    Logger::getInstance().log("Dirt level set successfully");
}

bool HouseLocation::isValidDirtLevel(uint8_t dirtLevel) {
    Logger::getInstance().log("Validating dirt level: " + std::to_string(dirtLevel));
    bool isValid = dirtLevel <= 9;
    Logger::getInstance().log("Dirt level validation result: " + std::to_string(isValid));
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