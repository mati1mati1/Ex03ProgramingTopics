#pragma once
#include "CleaningRecordStep.hpp"
#include <vector>
#include <memory>


class CleaningRecord{
    public:
        CleaningRecord(CleaningRecordStep initialStep, uint32_t maxSteps);
        void add(CleaningRecordStep step);
        const std::shared_ptr<CleaningRecordStep> getInitialStep() const;
        const std::shared_ptr<CleaningRecordStep> last() const;
        uint32_t size() const;
        const std::shared_ptr<CleaningRecordStep> operator[](std::size_t idx);
        uint32_t getMaxSteps() const {return MaxSteps;};
        friend std::ostream& operator<<(std::ostream& os, const CleaningRecord& record);
        
    private:
        bool hasInitialStep;
        uint32_t MaxSteps;
        std::vector<std::shared_ptr<CleaningRecordStep>> steps;
};

