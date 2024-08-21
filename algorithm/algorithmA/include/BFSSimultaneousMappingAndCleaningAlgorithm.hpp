#pragma once
#include "MappingAlgorithm.hpp"

class BFSSimultaneousMappingAndCleaningAlgorithm : public MappingAlgorithm {
public:
    virtual ~BFSSimultaneousMappingAndCleaningAlgorithm() {}

protected:
    std::optional<Step> findStepToNearestDirtyOrUnknownTile() const ;
    virtual Step calculateNextStep() override;
};