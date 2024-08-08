#pragma once
#include "MappingAlgorithm.hpp"
#include "AlgorithmConfig.hpp"

class BFSSimultaneousMappingAndCleaningAlgorithm : public MappingAlgorithm {
public:
    virtual ~BFSSimultaneousMappingAndCleaningAlgorithm() {}

protected:
    std::optional<Step> findStepToNearestDirtyOrUnknownTile() const ;
    virtual Step calculateNextStep() override;
};
class BFSSimultaneousMappingAndCleaningAlgorithmConfig : public AlgorithmConfig {
public:
    BFSSimultaneousMappingAndCleaningAlgorithmConfig() { };
    AlgorithmType getType() const override { return AlgorithmType::Simultaneous; };
    std::string getAlgorithmName() const override { return "SimultaneousMappingCleaningAlgorithm"; };
    ~BFSSimultaneousMappingAndCleaningAlgorithmConfig() override = default;
};