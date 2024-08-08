#include "BFSSimultaneousMappingAndCleaningAlgorithm.hpp"

std::optional<Step> BFSSimultaneousMappingAndCleaningAlgorithm::getStepTowardsClosestReachableTileToClean() const {
    if (!isExistsMappedCleanableTile()) {
        return std::nullopt;
    }
    auto [results, iterator] = noWallGraph.bfs_find_first(relativeCoordinates, std::function<bool(const Coordinate<int32_t>&, const BFSResult&)>(
        [&](const Coordinate<int32_t>& coordinate, const BFSResult& searchResult) {
            auto locationMapping = noWallGraph.getVertex(coordinate);
            bool canReachAndReturn = stepsUntilMustBeOnCharger(searchResult.getDistance()) + 1 > getLengthToCharger(coordinate);
            bool isDirtyTile = locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE &&
                               locationMapping.getHouseLocation().getDirtLevel() > 0;
            bool isUnmappedTile = locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN;                   
            return canReachAndReturn && (isDirtyTile || isUnmappedTile);
        }
    ));
    if (iterator == results->end()) {
        return std::nullopt;
    }
    Step step = getStepTowardsDestination(iterator->first, results);
    return step;
}

Step BFSSimultaneousMappingAndCleaningAlgorithm::calculateNextStep() {
    std::optional<Step> forcedMove = getForcedMove();
    if (forcedMove.has_value()) {
        return *forcedMove;
    }

    std::optional<Step> step = getStepTowardsClosestReachableTileToClean();
    if (!step.has_value()) {
        step = getStepTowardsClosestReachableUnknown();
    }
    if (step.has_value()) {
        return *step;
    }

    if (isOnCharger()) {
        finished = true;
        return Step::Finish;
    }
    return stepTowardsCharger();
}
