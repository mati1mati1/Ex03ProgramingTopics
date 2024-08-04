#pragma once
#include "AbstractMappingAlgorithm.hpp"
#include "AlgorithmConfig.hpp"

class MappingAlgorithm : public AbstractMappingAlgorithm {
public:
    virtual ~MappingAlgorithm() {}

protected:
    virtual std::optional<Step> getStepTowardsClosestReachableTileToClean() const override;
    virtual Step calculateNextStep() override;

private:
    bool isMappingStage() const;
};
class MappingAlgorithmConfig : public AlgorithmConfig {
public:
    MappingAlgorithmConfig() { };
    AlgorithmType getType() const override { return AlgorithmType::Mapping; };
    std::string getAlgorithmName() const override { return "MappingAlgorithm"; };
    ~MappingAlgorithmConfig() override = default;
};