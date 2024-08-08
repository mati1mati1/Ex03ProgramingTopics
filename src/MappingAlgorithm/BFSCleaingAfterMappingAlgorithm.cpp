#include "BFSCleaingAfterMappingAlgorithm.hpp"

std::optional<Step> BFSCleaingAfterMappingAlgorithm::getStepTowardsClosestReachableTileToClean() const {
    if (!isExistsMappedCleanableTile()) {
        return std::nullopt;
    }
    auto [results, iterator] = noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool(const Coordinate<int32_t>&, const BFSResult&)>(
        [&](const Coordinate<int32_t>& coordinate, const BFSResult& searchResult) {
            auto locationMapping = noWallGraph.getVertex(coordinate);
            bool canReachAndReturn = stepsUntilMustBeOnCharger(searchResult.getDistance()) > getLengthToCharger(coordinate);
            bool isDirtyTile = locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE &&
                               locationMapping.getHouseLocation().getDirtLevel() > 0;
            return canReachAndReturn && isDirtyTile;
        }
    ));
    if (iterator == results->end()) {
        return std::nullopt;
    }
    Step step = getStepTowardsDestination(iterator->first, results);
    return step;
}

Step BFSCleaingAfterMappingAlgorithm::calculateNextStep() {
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
        /**
            Note the REACHABLE part, we do not want to start moving to a tile only to run out of battery before we can clean it
            Thus, this function may return nullopt in the case where there isn't a useful move to be made
         */
        step = getStepTowardsClosestReachableUnknown();
    }
    else
    {
        step = getStepTowardsClosestReachableTileToClean();
        if (!step.has_value())
        {
            step = getStepTowardsClosestReachableUnknown();
        }
    }
    if (step.has_value())
    {
        return *step;
    }
    
    /**
        - If we got here, the battey is full
        - We are on the charger with a full battery and cant manage to find a useful move evident by the fact that
            - No cleanable, reachable tile was found
            - No unknown, reachable tile was found
        - We have full battery, thus we will never be able to make progress and this step should be finish.
     */
    if (isOnCharger())
    {
        finished = true;
        return Step::Finish;
    }
    return stepTowardsCharger();
    
}
bool BFSCleaingAfterMappingAlgorithm::isMappingStage() const {
    return  std::min(static_cast<uint32_t>(sqrt(maxSteps)),maxBattery) > stepsTaken && !isCompletelyMapped();
}