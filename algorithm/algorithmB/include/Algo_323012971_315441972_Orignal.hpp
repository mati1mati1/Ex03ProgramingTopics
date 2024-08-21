#pragma once
#include "MappingAlgorithm.hpp"

class Algo_323012971_315441972_Orignal : public MappingAlgorithm
{
public:
    virtual ~Algo_323012971_315441972_Orignal() {}

protected:
    std::optional<Step> findStepToNearestDirtyTile() const;
    virtual Step calculateNextStep() override;

private:
    bool isMappingStage() const;
};