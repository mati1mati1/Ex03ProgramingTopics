#pragma once
#include "CleaningRecord.hpp"
#include <memory>
class ScoreCalculator {
public:
    virtual int calculateScore(const std::shared_ptr<CleaningRecord> record, bool timedOut) = 0;
};
class VacuumScoreCalculator : public ScoreCalculator
{
public:
    int calculateScore(const std::shared_ptr<CleaningRecord> record, bool timedOut) override
    {
        uint32_t dirtScore = record->last()->getDirtLevel() * 300; 
        bool inDock = record->last()->isAtDockingStation();
        auto status = record->getStatus();
        if(timedOut){
            return record->getMaxSteps() * 2 + record->getInitialDirt() * 300 + 2000;
        }
        if (status == Status::DEAD)
        {
            return record->getMaxSteps() + dirtScore + 2000;
        }
        if (status == Status::FINISHED && !inDock)
        {
            return record->getMaxSteps() + dirtScore + 3000;
        }
        return record->size() + dirtScore + (inDock ? 0 : 1000);
    }
};