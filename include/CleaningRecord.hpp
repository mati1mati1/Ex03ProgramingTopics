#pragma once
#include "CleaningRecordStep.hpp"
#include <vector>
#include <memory>


class CleaningRecord{
    public:
        CleaningRecord(CleaningRecordStep initialStep);
        void add(CleaningRecordStep step);
        const std::shared_ptr<CleaningRecordStep> getInitialStep() const;
        const std::shared_ptr<CleaningRecordStep> last() const;
        uint32_t size() const;
        const std::shared_ptr<CleaningRecordStep> operator[](std::size_t idx);
        friend std::ostream& operator<<(std::ostream& os, const CleaningRecord& record);
        
    private:
        bool hasInitialStep;
        std::vector<std::shared_ptr<CleaningRecordStep>> steps;
};

