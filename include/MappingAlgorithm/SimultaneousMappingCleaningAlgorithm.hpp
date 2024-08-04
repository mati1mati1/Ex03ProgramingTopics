#pragma once
#include "AbstractMappingAlgorithm.hpp"
#include "AlgorithmConfig.hpp"

class SimultaneousMappingCleaningAlgorithm : public AbstractMappingAlgorithm {
public:
    virtual ~SimultaneousMappingCleaningAlgorithm() {}

protected:
    virtual std::optional<Step> getStepTowardsClosestReachableTileToClean() const override;
    virtual Step calculateNextStep() override;
};
class SimultaneousMappingCleaningAlgorithmConfig : public AlgorithmConfig {
public:
    SimultaneousMappingCleaningAlgorithmConfig() { };
    AlgorithmType getType() const override { return AlgorithmType::Simultaneous; };
    ~SimultaneousMappingCleaningAlgorithmConfig() override = default;
};