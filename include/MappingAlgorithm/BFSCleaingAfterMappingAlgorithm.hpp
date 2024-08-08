#pragma once
#include "MappingAlgorithm.hpp"
#include "AlgorithmConfig.hpp"

class BFSCleaingAfterMappingAlgorithm : public MappingAlgorithm {
public:
    virtual ~BFSCleaingAfterMappingAlgorithm() {}

protected:
    virtual std::optional<Step> getStepTowardsClosestReachableTileToClean() const override;
    virtual Step calculateNextStep() override;

private:
    bool isMappingStage() const;
};
class BFSCleaingAfterMappingAlgorithmConfig : public AlgorithmConfig {
public:
    BFSCleaingAfterMappingAlgorithmConfig() { };
    AlgorithmType getType() const override { return AlgorithmType::BFSCleaingAfterMappingAlgorithm; };
    std::string getAlgorithmName() const override { return "BFSCleaingAfterMappingAlgorithm"; };
    ~BFSCleaingAfterMappingAlgorithmConfig() override = default;
};