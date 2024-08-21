#pragma once
#include "MappingAlgorithm.hpp"

class BFSCleaingAfterMappingAlgorithm : public MappingAlgorithm
{
public:
    virtual ~BFSCleaingAfterMappingAlgorithm() {}

protected:
    std::optional<Step> findStepToNearestDirtyTile() const;
    virtual Step calculateNextStep() override;

private:
    bool isMappingStage() const;
};