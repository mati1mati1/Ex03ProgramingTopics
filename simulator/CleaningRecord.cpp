#include "CleaningRecord.hpp"

CleaningRecord::CleaningRecord(const CleaningRecordStep& initialStep, uint32_t maxSteps)
    : hasInitialStep(true), maxSteps(maxSteps) {
    steps = std::vector<std::shared_ptr<CleaningRecordStep>>();
    steps.emplace_back(std::make_shared<CleaningRecordStep>(initialStep));
}
Status CleaningRecord::getStatus() const
{
    if (isDead())
    {
        return Status::DEAD;
    }
    if (isFinished())
    {
        return Status::FINISHED;
    }
    return Status::WORKING;
}
const std::shared_ptr<CleaningRecordStep> CleaningRecord::operator[](std::size_t idx) {
    return steps.at(idx);
}

const std::shared_ptr<CleaningRecordStep> CleaningRecord::last() const {
    return steps.back();
}

void CleaningRecord::add(CleaningRecordStep step) {
    if (hasInitialStep) {
        steps.clear();
        hasInitialStep = false;
    }
    steps.emplace_back(std::make_shared<CleaningRecordStep>(step));
}

const std::shared_ptr<CleaningRecordStep> CleaningRecord::getInitialStep() const {
    if (hasInitialStep) {
        return steps[0];
    }
    return nullptr;
}

std::ostream& operator<<(std::ostream& os, const CleaningRecord& record) {
    if (!record.hasInitialStep) {
        for (const auto& step : record.steps) {
            os << *step;
        }
    }
    return os;
}

uint32_t CleaningRecord::size() const {
    if (hasInitialStep) {
        return 0;
    }
    
    if (last()->getStep() == Step::Finish) {
        return steps.size() - 1;
    }
    return steps.size();
}
uint32_t CleaningRecord::getInitialDirt(){
    if (hasInitialStep) {
        return 0;
    }
    return steps[0]->getDirtLevel();
}
