#include "CleaningRecord.hpp"
#include "Logger.hpp"

CleaningRecord::CleaningRecord(CleaningRecordStep initialStep) : hasInitialStep(true)
{
    Logger::getInstance().log("Initializing CleaningRecord");
    steps = std::vector<std::shared_ptr<CleaningRecordStep>>();
    steps.emplace_back(std::make_shared<CleaningRecordStep>(initialStep));
    
}

const std::shared_ptr<CleaningRecordStep> CleaningRecord::operator[](std::size_t idx)
{
    if (hasInitialStep) {
        Logger::getInstance().log("Attempted to access step at index " + std::to_string(idx) + " initial step is present.");
    }
    return steps.at(idx); 
}
const std::shared_ptr<CleaningRecordStep> CleaningRecord::last() const
{
    return steps.back();
}
void CleaningRecord::add(CleaningRecordStep step) {
    if (hasInitialStep) {
        steps.clear();
        hasInitialStep = false;
    }
    Logger::getInstance().log("Adding step to CleaningRecord : ");
    std::stringstream ss;
    ss << step;
    Logger::getInstance().log(ss.str());
    steps.emplace_back(std::make_shared<CleaningRecordStep>(step));
}
const std::shared_ptr<CleaningRecordStep> CleaningRecord::getInitialStep() const {
    if (hasInitialStep) {
        return steps[0];
    }
    return nullptr;
}


std::ostream& operator<<(std::ostream& os, const CleaningRecord& record) {
    if (!record.hasInitialStep) 
        for (const auto& step : record.steps) {
            os << *step;
    }
    return os;
}

uint32_t CleaningRecord::size() const{
    if (hasInitialStep) {
        return 0;
    }
    if (last()->getStep() == Step::Finish)
    {
        return steps.size() - 1;
    }
    
    return steps.size();
}
