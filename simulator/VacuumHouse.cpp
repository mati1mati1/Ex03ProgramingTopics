#include "VacuumHouse.hpp"
#include <stdexcept>
#include <sstream>
#include <cassert>

template <typename T>
bool inVectorBounds(const std::vector<std::vector<T>> &locations, uint32_t i, uint32_t j)
{
    return i < locations.size() && j < locations[i].size();
}
bool inVectorStringBounds (const std::vector<std::string>& locations, uint32_t i, uint32_t j)
{
    return i < locations.size() && j < locations[i].size();
}

bool VacuumHouse::inBounds(Coordinate<size_t> &coordinate) const
{
    return inVectorBounds(houseLocations, coordinate.getX(), coordinate.getY());
}
std::vector<std::vector<HouseLocation>> VacuumHouse::constructHouseLocation(const std::vector<std::string> &locations, uint32_t rows, uint32_t cols)
{
    std::vector<std::vector<HouseLocation>> houseLocations;
    for (size_t i = 0; i < rows; i++) {
        std::vector<HouseLocation> houseRow;
        for (size_t j = 0; j < cols; j++) {
            char location_represatation = ' ';
            if(inVectorStringBounds(locations,i,j)){
                location_represatation = locations[i][j];
            }
            HouseLocation location = HouseLocation(location_represatation);
            houseRow.push_back(location);
        }
        houseLocations.push_back(houseRow);
    }
    return houseLocations;
}

Coordinate<size_t> findChargingStation(std::vector<std::vector<HouseLocation>>& houseLocations, uint32_t rows, uint32_t cols) {
    HouseLocation* chargingStationLocation = nullptr;
    Coordinate<size_t>* chargingStationCoordinate = nullptr;
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            if (houseLocations[i][j].getLocationType() == LocationType::CHARGING_STATION) {
                if (chargingStationLocation != nullptr) {
                    throw std::invalid_argument("Multiple charging stations found in house");
                }
                chargingStationLocation = &houseLocations[i][j];
                chargingStationCoordinate = new Coordinate(i,j);
            }
        }
    }
    if (chargingStationLocation == nullptr) {
        throw std::invalid_argument("No charging station found in house");
    }
    std::stringstream ss;
    ss << *chargingStationLocation;
    return *chargingStationCoordinate;
}

HouseLocation& VacuumHouse::getDirectionLocation(const Direction& direction)
{
    Step step = DirectionTools::toStep(direction);
    return getStepLocation(step);
}
const HouseLocation &VacuumHouse::getDirectionLocation(const Direction &direction) const
{
    auto& temp =  const_cast<VacuumHouse*>(this)->getDirectionLocation(direction);
    return temp;
}

HouseLocation& VacuumHouse::getStepLocation(const Step &step)
{
    auto coordinate = currentLocation.getStep(step);
    if (!inBounds(coordinate))
    {
        return logicalWall;
    }

    return houseLocations[coordinate.getX()][coordinate.getY()];
};

std::ostream& operator<<(std::ostream& os, const VacuumHouse& house)
{
    for (const auto& row : house.houseLocations) {
        for (const auto& location : row) {
            os << location;
        }
        os << std::endl;
    }
    return os;
}

VacuumHouse::VacuumHouse(const std::vector<std::string>& locations,uint32_t rows,uint32_t cols) :logicalWall(LocationType::WALL){

    houseLocations = constructHouseLocation(locations,rows,cols);
    auto coordinates = findChargingStation(houseLocations,rows,cols);
    chargingStation = &houseLocations[coordinates.getX()][coordinates.getY()];
    currentLocation = coordinates;
}
uint32_t VacuumHouse::getTotalDirt() const{
    uint32_t totalDirt = 0;
    for (const auto& row : houseLocations) {
        for (const auto& location : row) {
            if (location.getDirtLevel()) {
                totalDirt += location.getDirtLevel();
            }
        }
    }
    return totalDirt;
}
const HouseLocation& VacuumHouse::getCurrentLocation() const{
    return const_cast<VacuumHouse*>(this)->getCurrentLocation();
}

HouseLocation& VacuumHouse::getCurrentLocation(){
    return houseLocations[currentLocation.getX()][currentLocation.getY()];
}
bool VacuumHouse::is_move(const Step& step)
{
    return DirectionTools::isStayInPlaceStep(step) || is_move(DirectionTools::reduceStepToDirection(step));
}
bool VacuumHouse::is_move(const Direction& direction)
{
    auto newLocation = getDirectionLocation(direction);
    if (&newLocation == &logicalWall || newLocation.getLocationType() == LocationType::WALL){
        return false;
    }

    return true;
}
void VacuumHouse::move(const Direction& direction)
{
    if (!is_move(direction)) {
        std::cerr << "Error: Attempted to move into wall" << std::endl;
        return;
    }
    currentLocation = currentLocation.getDirection(direction);
}
void VacuumHouse::move(const Step& direction)
{
    if (DirectionTools::isStayInPlaceStep(direction)) {
        return;
    }
    move(DirectionTools::reduceStepToDirection(direction));

}

void VacuumHouse::cleanCurrentLocation() {
    auto& location = getCurrentLocation();
    if (location.getLocationType() == LocationType::HOUSE_TILE){
        location.setDirtLevel(std::max(location.getDirtLevel() - 1,0));
    }
}