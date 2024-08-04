#include "MappingAlgorithm/MappingAlgorithm.hpp"
#include <algorithm>
#include "Coordinate.hpp"
#include "MappingAlgorithm.hpp"
#include <Logger.hpp>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

Step MappingAlgorithm::calculateNextStep()
{
    /*
        Before we even start calculating the next step we need to check if we are in a forced move situation
        Forced moves are:
            * when returning to the charges is a must (Any other more will result in DEAD)
            * We theoretically could make progress given the current mapping (Clean a tile, expose an unmapped tile that could be cleaned in the future)
            * when the battery is NOT full and we are on the charger 
            * Finishing correctly, we are on the charger and for any reason no more progress is possible
     */
    std::optional<Step> forcedMove = getForcedMove();
    if (forcedMove.has_value())
    {
        return *forcedMove;
    }
    /*
        The algorithm is seperated into the mapping stage and the cleaning stage
        In the mapping stage we do not clean unless we have exposed the entire house, which automatically ends the mapping stage
        In the cleaning stage we clean any tile we are on to completion moving on the next closest tile
    */
    std::optional<Step> step;
    if(isMappingStage())
    {
        Logger::getInstance().log("Info: Mapping stage");
        /**
            Note the REACHABLE part, we do not want to start moving to a tile only to run out of battery before we can clean it
            Thus, this function may return nullopt in the case where there isn't a useful move to be made
         */
        step = getStepTowardsClosestReachableUnknown();
    }
    else
    {
        Logger::getInstance().log("Info: Cleaning stage");
        step = getStepTowardsClosestReachableTileToClean();
        if (!step.has_value())
        {
            Logger::getInstance().log("Info: No cleanable tiles found, attempting to map new tiles");
            step = getStepTowardsClosestReachableUnknown();
        }
    }
    if (step.has_value())
    {
        return *step;
    }
    
    Logger::getInstance().log("Info: No progress possible, returning to charger for termination");
    /**
        - If we got here, the battey is full
        - We are on the charger with a full battery and cant manage to find a useful move evident by the fact that
            - No cleanable, reachable tile was found
            - No unknown, reachable tile was found
        - We have full battery, thus we will never be able to make progress and this step should be finish.
     */
    if (isOnCharger())
    {
        Logger::getInstance().log("Info: Already on charger, terminating");
        finished = true;
        return Step::Finish;
    }
    return stepTowardsCharger();
    
    
    
}
std::optional<Step> MappingAlgorithm::getForcedMove() const{

    /**
        Could a state be reached in the future where a route can be made to make progress?
     */
    bool isProgressPossibleTheoreticallyCache  = isProgressPossibleTheoretically();

    
    if ((!isProgressPossibleTheoreticallyCache && isOnCharger()))
    {
        /*
            All reachable and cleanable tiles were handled, we are on the charger, terminate properly
        */
        Logger::getInstance().log("Info: Algorithm terminated properly");
        return Step::Finish;
    }
    if (!isProgressPossibleTheoreticallyCache || mustReturnToCharger())
    {
        /*
            There is no move possible that wouldn't make you dead in the future except going to the charger in the quickest way
        */
        Logger::getInstance().log("Info: No progress possible, returning to charger for termination");
        return stepTowardsCharger();
    }
    if (isOnCharger() && !isFullyCharged())
    {
        /*
            Charge to full with the caveat that you shouldn't charge more battery than you have steps left.
        */
        Logger::getInstance().log("Info: battery is not full, charging");
        return Step::Stay;
    }
    return std::nullopt;
}
[[nodiscard]] Step MappingAlgorithm::nextStep() {
    
    if (!isSensorsSet())
    {
        Logger::getInstance().log("Error: No sensors terminating");
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
bool MappingAlgorithm::isMappingStage() const {
    return  std::min(static_cast<uint32_t>(sqrt(maxSteps)),maxBattery) > stepsTaken && !isCompletelyMapped();
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
    return std::min(battery,localMaxSteps);
}
/**
 * Different from returnToCharger if you intend to stay which will not increase your distance the next time
 */

bool MappingAlgorithm::mustReturnToCharger() const
{
    auto [results,iterator]= noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool (const Coordinate<int32_t> &, const BFSResult &)> (
        [&](const Coordinate<int32_t> &coordinate, const BFSResult &){
            return coordinate == getChargerLocation();
        }
    ));
    return iterator->second.getDistance() >= stepsUntilMustBeOnCharger(0);
}

Step MappingAlgorithm::stepTowardsCharger() const
{
    auto [results,iterator]= noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool (const Coordinate<int32_t> &, const BFSResult &)> (
        [&](const Coordinate<int32_t> &coordinate, const BFSResult &){
            return coordinate == getChargerLocation();
        }
    ));
    if(iterator == results->end())
    {
        throw std::runtime_error("Could not find path to charger in BFS results");
    }
    Step step = getStepTowardsDestination(getChargerLocation(),results);
    return step;
}
uint32_t MappingAlgorithm::getLengthToCharger(Coordinate<int32_t> from) const
{
    auto result = noWallGraph.bfs(getChargerLocation());
    
    if(result == nullptr || !result->contains(from))
    {
        throw std::runtime_error("Could not find path to charger in BFS results");
    }
    return result->at(from).getDistance();
}
bool MappingAlgorithm::isExistsMappedCleanableTile() const
{
    for (const auto& mapping : noWallGraph.getMappings())
    {
        if(mapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE && mapping.getHouseLocation().getDirtLevel() > 0)
        {
            return true;
        }
    }
    return false;
}
std::optional<Step> MappingAlgorithm::getStepTowardsClosestReachableTileToClean() const
{
    if(!isExistsMappedCleanableTile())
    {
        return std::nullopt;
    }
    auto [results,iterator] = noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool (const Coordinate<int32_t> &, const BFSResult &)> (
        [&](const Coordinate<int32_t> &coordinate, const BFSResult & bfsResult){
            auto locationMapping = noWallGraph.getVertex(coordinate);
            return stepsUntilMustBeOnCharger(bfsResult.getDistance()) + 1 >= getLengthToCharger(coordinate) &&
                locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE && locationMapping.getHouseLocation().getDirtLevel() > 0;
        }
    ));
    if(iterator == results->end())
    {
        return std::nullopt;
    }
    Step step = getStepTowardsDestination(iterator->first,results);
    return step;

}
bool MappingAlgorithm::isCompletelyMapped() const
{
    if(isCompletelyMappedCache)
    {
        return true;
    }
    auto [results,iterator] = noWallGraph.bfs_find_first(getChargerLocation(), std::function<bool (const Coordinate<int32_t> &, const BFSResult &)> (
        [&](const Coordinate<int32_t> &coordinate, const BFSResult & bfsResult){
            auto locationMapping = noWallGraph.getVertex(coordinate);
            return isPotentiallyCleanableTile(locationMapping,bfsResult);
        }
    ));
    isCompletelyMappedCache = iterator == results->end();
    return isCompletelyMappedCache;
}
bool MappingAlgorithm::isProgressPossibleTheoretically() const
{
    auto [results,iterator] = noWallGraph.bfs_find_first(getChargerLocation(), std::function<bool (const Coordinate<int32_t> &, const BFSResult &)> (
        [&](const Coordinate<int32_t> &coordinate, const BFSResult & bfsResult){
            auto locationMapping = noWallGraph.getVertex(coordinate);
            return (isKnownCleanableTile(locationMapping,bfsResult) || isPotentiallyCleanableTile(locationMapping,bfsResult));
        }
    ));
    return iterator != results->end();
}
bool MappingAlgorithm::isAtMaxSteps() const
{
    return stepsTaken >= maxSteps;
}
bool MappingAlgorithm::isKnownCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const
{
    return locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE &&locationMapping.getHouseLocation().getDirtLevel() > 0 && result.getDistance() <= maxCleanableDistance();
}
bool MappingAlgorithm::isPotentiallyCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const
{
    return locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN && (result.getDistance() <= maxCleanableDistance());
}
std::optional<Step> MappingAlgorithm::getStepTowardsClosestReachableUnknown() const
{
    auto [results,iterator] = noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool (const Coordinate<int32_t> &, const BFSResult &)> (
        [&](const Coordinate<int32_t> &coordinate, const BFSResult &bfsResult){
            auto locationMapping = noWallGraph.getVertex(coordinate);
            return stepsUntilMustBeOnCharger(bfsResult.getDistance()) >= getLengthToCharger(coordinate) && locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN;
        }
    ));
    if(iterator == results->end())
    {
        return std::nullopt;
    }
    Step step = getStepTowardsDestination(iterator->first,results);
    return step;
}
Step MappingAlgorithm::getStepTowardsDestination(const Coordinate<int32_t>& destination, const std::shared_ptr<std::unordered_map<Coordinate<int32_t>,BFSResult>> results) const
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
    HouseLocation location = HouseLocation(LocationType::HOUSE_TILE,dirtSensor->dirtLevel());
    updateLocationIfExists(location);
    if (noWallGraph.isVertex(relativeCoordinates))
    {
        return;
    }

    if (isOnCharger())
    {
        location = HouseLocation(LocationType::CHARGING_STATION);
    }
    noWallGraph.addVertex(HouseLocationMapping(relativeCoordinates,location));
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
            noWallGraph.addVertex(HouseLocationMapping(newLocationPair,location));
        }
    }
    /**
     * A vertex may not be new but does need to attach an edge to it as it is dicovered as unknown by another vertex promping the need to attach an edge to it for another location
     */
    if(location.getLocationType() != LocationType::WALL)
    {
        noWallGraph.addEdge(relativeCoordinates,direction);
    }
    
}
void MappingAlgorithm::updateLocationIfExists(const HouseLocation& newLocation)
{
    if (!noWallGraph.isVertex(relativeCoordinates))
    {
        return;
    }
    auto& vertex = noWallGraph.getVertex(relativeCoordinates);
    auto& noWallVertex = noWallGraph.getVertex(relativeCoordinates);
    vertex.update(newLocation);
    noWallVertex.update(newLocation);
    
}
void MappingAlgorithm::mapSurroundings()
{
    mapCurrentLocation();
    mapDirection(Direction::East);
    mapDirection(Direction::West);
    mapDirection(Direction::North);
    mapDirection(Direction::South);
}

