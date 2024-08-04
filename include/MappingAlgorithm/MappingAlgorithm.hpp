#pragma once
#include "abstract_algorithm.h"
#include "AlgorithmConfig.hpp"
#include "MappingGraph.hpp"
#include <Coordinate.hpp>
#include <HouseLocation.hpp>
class MappingAlgorithm : public AbstractAlgorithm {
public:
    MappingAlgorithm() {};
    ~MappingAlgorithm() override = default;
    void setMaxSteps(std::size_t maxSteps) override {this->maxSteps = maxSteps;};
    void setWallsSensor(const WallsSensor& sensor) override { wallsSensor =&sensor;};
    void setDirtSensor(const DirtSensor& sensor) override { dirtSensor = &sensor;};
    void setBatteryMeter(const BatteryMeter& meter) override { batteryMeter = &meter; maxBattery = batteryMeter->getBatteryState();};
    Step nextStep() override;


private:
    constexpr uint32_t maxReachableDistance() const { return maxBattery / 2 ;};
    constexpr uint32_t maxCleanableDistance() const { return (maxBattery-1) /2 ;};
    const Coordinate<int32_t> CHRAGER_LOCATION = Coordinate<int32_t>(0,0);
    const Coordinate<int32_t>& getChargerLocation() const { return CHRAGER_LOCATION;};
private:
    uint32_t maxBattery = 0;
    uint32_t maxSteps = 0;
    uint32_t stepsTaken = 0;
    const BatteryMeter *batteryMeter = nullptr;
    const WallsSensor *wallsSensor = nullptr;
    const DirtSensor* dirtSensor= nullptr;
    MappingGraph noWallGraph;
    Coordinate<int32_t> relativeCoordinates = Coordinate<int32_t>(0,0);
    mutable bool finished = false;
    mutable bool isCompletelyMappedCache = false;
private:
    Step stepTowardsCharger() const;
    std::optional<Step> getStepTowardsClosestReachableTileToClean() const;
    std::optional<Step> getStepTowardsClosestReachableUnknown() const;
    std::optional<Step> getForcedMove() const;
    uint32_t getLengthToCharger(Coordinate<int32_t> from) const;
    bool isExistsMappedCleanableTile() const;
    bool isCompletelyMapped() const;
    bool isFullyCharged() const;
    bool mustReturnToCharger() const;
    uint32_t stepsUntilMustBeOnCharger(uint32_t offset = 0) const;
    bool isWorthWhileStep(Step step) const;
    bool isProgressPossibleTheoretically() const;
    bool isKnownCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const;
    bool isPotentiallyCleanableTile(const HouseLocationMapping &locationMapping, BFSResult result) const;
    void updateLocationIfExists(const HouseLocation &newLocation);
    Step getStepTowardsDestination(const Coordinate<int32_t>& destination, const std::shared_ptr<std::unordered_map<Coordinate<int32_t>,BFSResult>> results) const;
    bool isSensorsSet() const { return wallsSensor && dirtSensor && batteryMeter;};
    bool isOnCharger() const;
    Step calculateNextStep();
    bool isAtMaxSteps() const;
    bool isMappingStage() const;
    void mapDirection(Direction direction);
    void mapSurroundings();
    void mapCurrentLocation();
};
class MappingAlgorithmConfig : public AlgorithmConfig {
public:
    MappingAlgorithmConfig() { };
    AlgorithmType getType() const override { return AlgorithmType::Mapping; };
    ~MappingAlgorithmConfig() override = default;
};