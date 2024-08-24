#include "Algo_323012971_315441972_Simultaneous.hpp"
#include "AlgorithmRegistration.h"

std::optional<Step> Algo_323012971_315441972_Simultaneous::findStepToNearestDirtyOrUnknownTile() const {
    if (!isExistsMappedCleanableTile()) {
        return std::nullopt;
    }
    const auto& graph = getNoWallGraph();
    auto condition =  [&](const Coordinate<int32_t>& coordinate, const BFSResult& searchResult) {
        auto locationMapping = graph.getVertex(coordinate);
        bool canReachAndReturn = stepsUntilMustBeOnCharger(searchResult.getDistance()) + 1 > getLengthToCharger(coordinate);
        bool isDirtyTile = locationMapping.getHouseLocation().getLocationType() == LocationType::HOUSE_TILE &&
                           locationMapping.getHouseLocation().getDirtLevel() > 0;
        bool isUnmappedTile = locationMapping.getHouseLocation().getLocationType() == LocationType::UNKNOWN;                   
        return canReachAndReturn && (isDirtyTile || isUnmappedTile);
        };

    auto step = findStepToNearestMatchingTile(condition);

    if (step.has_value())
    {
        return step;
    }
    return std::nullopt;
}


Step Algo_323012971_315441972_Simultaneous::calculateNextStep() {
    if (auto forcedMove = getForcedMove()) {
        return *forcedMove;
    }

    if (auto step = findStepToNearestDirtyOrUnknownTile()) {
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
REGISTER_ALGORITHM(Algo_323012971_315441972_Simultaneous);
