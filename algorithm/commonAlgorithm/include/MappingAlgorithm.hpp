#pragma once
#include "AbstractAlgorithm.h"
#include "MappingGraph.hpp"
#include "Coordinate.hpp"
#include "HouseLocation.hpp"
class MappingAlgorithm : public AbstractAlgorithm
{
public:
    ~MappingAlgorithm() override = default;
    void setMaxSteps(std::size_t maxSteps) override { this->maxSteps = maxSteps; };
    void setWallsSensor(const WallsSensor &sensor) override { wallsSensor = &sensor; };
    void setDirtSensor(const DirtSensor &sensor) override { dirtSensor = &sensor; };
    void setBatteryMeter(const BatteryMeter &meter) override
    {
        batteryMeter = &meter;
        maxBattery = batteryMeter->getBatteryState();
    };
    Step nextStep() override;

protected:
    virtual Step calculateNextStep() { return Step::Finish; };
    virtual std::optional<Step> getForcedMove() const;

    std::optional<Step> getStepTowardsClosestReachableUnknown() const;

    Step stepTowardsCharger() const;
    Step getStepTowardsDestination(const Coordinate<int32_t> &destination, const std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>> results) const;

    const MappingGraph &getNoWallGraph() const { return noWallGraph; }

    uint32_t stepsUntilMustBeOnCharger(uint32_t offset = 0) const;
    uint32_t getLengthToCharger(Coordinate<int32_t> from) const;
    uint32_t getMaxSteps() const { return maxSteps; };
    uint32_t getMaxBattery() const { return maxBattery; };
    uint32_t getStepsTaken() const { return stepsTaken; };

    void setFinished() { finished = true; };
    bool isCompletelyMapped() const;
    bool isExistsMappedCleanableTile() const;
    bool isOnCharger() const;

    std::optional<Step> findStepToNearestMatchingTile(const std::function<bool(const Coordinate<int32_t> &, const BFSResult &)> &predicate) const;

private:
    constexpr uint32_t maxReachableDistance() const { return maxBattery / 2; };
    constexpr uint32_t maxCleanableDistance() const { return (maxBattery - 1) / 2; };
    std::optional<Step> getStepTowardsClosestReachableTileToClean() const { return std::nullopt; };
    const Coordinate<int32_t> CHRAGER_LOCATION = Coordinate<int32_t>(0, 0);
    const Coordinate<int32_t> &getChargerLocation() const { return CHRAGER_LOCATION; };

    uint32_t maxBattery = 0;
    uint32_t maxSteps = 0;
    uint32_t stepsTaken = 0;

    const BatteryMeter *batteryMeter = nullptr;
    const WallsSensor *wallsSensor = nullptr;
    const DirtSensor *dirtSensor = nullptr;

    MappingGraph noWallGraph;

    Coordinate<int32_t> relativeCoordinates = Coordinate<int32_t>(0, 0);

    mutable bool finished = false;
    mutable bool isCompletelyMappedCache = false;

    bool isFullyCharged() const;
    bool mustReturnToCharger() const;
    bool isWorthWhileStep(Step step) const;
    bool isProgressPossibleTheoretically() const;
    bool isKnownCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const;
    bool isPotentiallyCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const;
    void updateLocationIfExists(const HouseLocation &newLocation);
    bool isSensorsSet() const { return wallsSensor && dirtSensor && batteryMeter; };
    bool isAtMaxSteps() const;

    void mapDirection(Direction direction);
    void mapSurroundings();
    void mapCurrentLocation();
};
