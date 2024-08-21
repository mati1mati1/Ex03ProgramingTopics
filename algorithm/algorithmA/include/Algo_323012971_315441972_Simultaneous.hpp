#pragma once
#include "MappingAlgorithm.hpp"

class Algo_323012971_315441972_Simultaneous : public MappingAlgorithm {
public:
    virtual ~Algo_323012971_315441972_Simultaneous() {}

protected:
    std::optional<Step> findStepToNearestDirtyOrUnknownTile() const ;
    virtual Step calculateNextStep() override;
};