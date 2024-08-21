#include "MappingAlgorithm.hpp"
#include <algorithm>
#include "Coordinate.hpp"
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

std::optional<Step> MappingAlgorithm::getForcedMove() const
{

    /**
        Could a state be reached in the future where a route can be made to make progress?
     */
    bool isProgressPossibleTheoreticallyCache = isProgressPossibleTheoretically();

    if ((!isProgressPossibleTheoreticallyCache && isOnCharger()))
    {
        /*
            All reachable and cleanable tiles were handled, we are on the charger, terminate properly
        */
        return Step::Finish;
    }
    if (!isProgressPossibleTheoreticallyCache || mustReturnToCharger())
    {
        /*
            There is no move possible that wouldn't make you dead in the future except going to the charger in the quickest way
        */
        return stepTowardsCharger();
    }
    if (isOnCharger() && !isFullyCharged())
    {
        /*
            Charge to full with the caveat that you shouldn't charge more battery than you have steps left.
        */
        return Step::Stay;
    }
    return std::nullopt;
}
[[nodiscard]] Step MappingAlgorithm::nextStep()
{

    if (!isSensorsSet())
    {
        throw std::runtime_error("Sensors not set, terminating algorithm");
    }

    /*
        This is handled here, we dont want any change to the algorithm to allow us to continue after we are finished or over max steps
    */
    if (finished || isAtMaxSteps())
    {
        return Step::Finish;
    }
    /*
        This is free and should be done at every turn so it is not a part of calculate
    */
    mapSurroundings();

    /*
        This function is the bulk of the logic, take a look at the documentation inside
    */
    Step step = calculateNextStep();
    if (step == Step::Finish)
    {
        finished = true;
    }
    /*
        Update the algorithm internal state to fit the move made
    */
    stepsTaken++;
    relativeCoordinates = relativeCoordinates.getStep(step);
    return step;
}

bool MappingAlgorithm::isFullyCharged() const
{
    return batteryMeter->getBatteryState() == maxBattery || batteryMeter->getBatteryState() >= maxSteps - stepsTaken;
}
bool MappingAlgorithm::isOnCharger() const
{
    return relativeCoordinates == getChargerLocation();
}
uint32_t MappingAlgorithm::stepsUntilMustBeOnCharger(uint32_t offeset) const
{
    uint32_t battery = 0;
    uint32_t localMaxSteps = 0;
    if (batteryMeter->getBatteryState() > offeset)
    {
        battery = batteryMeter->getBatteryState() - offeset;
    }
    if (maxSteps > (stepsTaken + offeset))
    {
        localMaxSteps = maxSteps - (stepsTaken + offeset);
    }
    return std::min(battery, localMaxSteps);
}
/**
 * Different from returnToCharger if you intend to stay which will not increase your distance the next time
 */

bool MappingAlgorithm::mustReturnToCharger() const
{
    auto [results, iterator] = noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool(const Coordinate<int32_t> &, const BFSResult &)>(
                                                                                   [&](const Coordinate<int32_t> &coordinate, const BFSResult &)
                                                                                   {
                                                                                       return coordinate == getChargerLocation();
                                                                                   }));
    return iterator->second.getDistance() >= stepsUntilMustBeOnCharger(0);
}

Step MappingAlgorithm::stepTowardsCharger() const
{
    auto condition = [&](const Coordinate<int32_t> &coordinate, const BFSResult &)
    { return coordinate == getChargerLocation(); };
    auto step = findStepToNearestMatchingTile(condition);
    if (step.has_value())
    {
        return step.value();
    }
    throw std::runtime_error("Could not find path to charger in BFS results");
}
uint32_t MappingAlgorithm::getLengthToCharger(Coordinate<int32_t> from) const
{
    auto result = noWallGraph.bfs(getChargerLocation());

    if (result == nullptr || !result->contains(from))
    {
        throw std::runtime_error("Could not find path to charger in BFS results");
    }
    return result->at(from).getDistance();
}
bool MappingAlgorithm::isExistsMappedCleanableTile() const
{
    for (const auto &mapping : noWallGraph.getMappings())
    {
        if (mapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE && mapping.getHouseLocation().getDirtLevel() > 0)
        {
            return true;
        }
    }
    return false;
}
bool MappingAlgorithm::isCompletelyMapped() const
{
    if (isCompletelyMappedCache)
    {
        return true;
    }
    auto [results, iterator] = noWallGraph.bfs_find_first(getChargerLocation(), std::function<bool(const Coordinate<int32_t> &, const BFSResult &)>(
                                                                                    [&](const Coordinate<int32_t> &coordinate, const BFSResult &bfsResult)
                                                                                    {
                                                                                        auto locationMapping = noWallGraph.getVertex(coordinate);
                                                                                        return isPotentiallyCleanableTile(locationMapping, bfsResult);
                                                                                    }));
    isCompletelyMappedCache = iterator == results->end();
    return isCompletelyMappedCache;
}
bool MappingAlgorithm::isProgressPossibleTheoretically() const
{
    auto [results, iterator] = noWallGraph.bfs_find_first(getChargerLocation(), std::function<bool(const Coordinate<int32_t> &, const BFSResult &)>(
                                                                                    [&](const Coordinate<int32_t> &coordinate, const BFSResult &bfsResult)
                                                                                    {
                                                                                        auto locationMapping = noWallGraph.getVertex(coordinate);
                                                                                        return (isKnownCleanableTile(locationMapping, bfsResult) || isPotentiallyCleanableTile(locationMapping, bfsResult));
                                                                                    }));
    return iterator != results->end();
}
bool MappingAlgorithm::isAtMaxSteps() const
{
    return stepsTaken >= maxSteps;
}
bool MappingAlgorithm::isKnownCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const
{
    return locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE && locationMapping.getHouseLocation().getDirtLevel() > 0 && result.getDistance() <= maxCleanableDistance();
}
bool MappingAlgorithm::isPotentiallyCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const
{
    return locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN && (result.getDistance() <= maxCleanableDistance());
}
std::optional<Step> MappingAlgorithm::getStepTowardsClosestReachableUnknown() const
{
    auto condition = [&](const Coordinate<int32_t> &coordinate, const BFSResult &bfsResult)
    {
                    auto locationMapping = noWallGraph.getVertex(coordinate);
                    return stepsUntilMustBeOnCharger(bfsResult.getDistance()) >= getLengthToCharger(coordinate)
                     && locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN; };
    auto step = findStepToNearestMatchingTile(condition);
    if (step.has_value())
    {
        return step;
    }
    return std::nullopt;
}
Step MappingAlgorithm::getStepTowardsDestination(const Coordinate<int32_t> &destination, const std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>> results) const
{
    if (relativeCoordinates == destination)
    {
        return Step::Stay;
    }
    auto vertexIterator = results->find(destination);
    if (vertexIterator == results->end())
    {
        throw std::runtime_error("Could not find path to destinationv in BFS results");
    }
    while (vertexIterator->second.getDistance() != 1)
    {
        auto nextStepCoordinates = vertexIterator->second.getParent();
        if (!nextStepCoordinates)
        {
            throw std::runtime_error("Could not find parent of node that is not root");
        }
        vertexIterator = results->find(*nextStepCoordinates);
        if (vertexIterator == results->end())
        {
            throw std::runtime_error("Could not find parent of node that is not root");
        }
    }
    Direction direction = relativeCoordinates.getDirection((*vertexIterator).first);
    return DirectionTools::toStep(direction);
}
void MappingAlgorithm::mapCurrentLocation()
{
    HouseLocation location = HouseLocation(LocationType::HOUSE_TILE, dirtSensor->dirtLevel());
    updateLocationIfExists(location);
    if (noWallGraph.isVertex(relativeCoordinates))
    {
        return;
    }

    if (isOnCharger())
    {
        location = HouseLocation(LocationType::CHARGING_STATION);
    }
    noWallGraph.addVertex(HouseLocationMapping(relativeCoordinates, location));
}
void MappingAlgorithm::mapDirection(Direction direction)
{
    auto newLocationPair = relativeCoordinates.getDirection(direction);
    HouseLocation location = HouseLocation(LocationType::UNKNOWN);
    if (wallsSensor->isWall(direction))
    {
        location = HouseLocation(LocationType::WALL);
    }
    if (!noWallGraph.isVertex(newLocationPair))
    {
        if (location.getLocationType() != LocationType::WALL)
        {
            noWallGraph.addVertex(HouseLocationMapping(newLocationPair, location));
        }
    }
    /**
     * A vertex may not be new but does need to attach an edge to it as it is dicovered as unknown by another vertex promping the need to attach an edge to it for another location
     */
    if (location.getLocationType() != LocationType::WALL)
    {
        noWallGraph.addEdge(relativeCoordinates, direction);
    }
}
void MappingAlgorithm::updateLocationIfExists(const HouseLocation &newLocation)
{
    if (!noWallGraph.isVertex(relativeCoordinates))
    {
        return;
    }
    auto &vertex = noWallGraph.getVertex(relativeCoordinates);
    auto &noWallVertex = noWallGraph.getVertex(relativeCoordinates);
    vertex.update(newLocation);
    noWallVertex.update(newLocation);
}

std::optional<Step> MappingAlgorithm::findStepToNearestMatchingTile(const std::function<bool(const Coordinate<int32_t> &, const BFSResult &)> &predicate) const
{

    auto [results, iterator] = noWallGraph.bfs_find_first(relativeCoordinates, predicate);

    if (iterator == results->end())
    {
        return std::nullopt;
    }

    Step step = getStepTowardsDestination(iterator->first, results);
    return step;
}

void MappingAlgorithm::mapSurroundings()
{
    mapCurrentLocation();
    mapDirection(Direction::East);
    mapDirection(Direction::West);
    mapDirection(Direction::North);
    mapDirection(Direction::South);
}
