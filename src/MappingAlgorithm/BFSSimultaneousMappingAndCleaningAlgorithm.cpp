#include "BFSSimultaneousMappingAndCleaningAlgorithm.hpp"

std::optional<Step> BFSSimultaneousMappingAndCleaningAlgorithm::getStepTowardsClosestReachableTileToClean() const {
    if (!isExistsMappedCleanableTile()) {
        return std::nullopt;
    }
    const auto& relativeCoordinates = GetRelativeCoordinates();
    const auto& graph = getNoWallGraph();
    auto [results, iterator] = graph.bfs_find_first(relativeCoordinates, [&](const Coordinate<int32_t>& coordinate, const BFSResult& searchResult) {
        auto locationMapping = graph.getVertex(coordinate);
        bool canReachAndReturn = stepsUntilMustBeOnCharger(searchResult.getDistance()) + 1 > getLengthToCharger(coordinate);
        bool isDirtyTile = locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE &&
                           locationMapping.getHouseLocation().getDirtLevel() > 0;
        bool isUnmappedTile = locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN;                   
        return canReachAndReturn && (isDirtyTile || isUnmappedTile);
    });
    if (iterator == results->end()) {
        return std::nullopt;
    }
    Step step = getStepTowardsDestination(iterator->first, results);
    return step;
}

Step BFSSimultaneousMappingAndCleaningAlgorithm::calculateNextStep() {
    if (auto forcedMove = getForcedMove()) {
        return *forcedMove;
    }

    if (auto step = getStepTowardsClosestReachableTileToClean()) {
        return *step;
    }

    if (auto step = getStepTowardsClosestReachableUnknown()) {
        return *step;
    }

    if (isOnCharger()) {
        setFinished();
        return Step::Finish;
    }
    return stepTowardsCharger();
}
