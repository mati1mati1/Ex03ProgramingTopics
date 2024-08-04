#pragma once
#include "CleaningRecordStep.hpp"
#include <vector>
#include <memory>
#include <string>

class CleaningRecord {
public:
    CleaningRecord(const CleaningRecordStep& initialStep, uint32_t maxSteps, const std::string& algorithmName);
    void add(CleaningRecordStep step);
    const std::shared_ptr<CleaningRecordStep> getInitialStep() const;
    const std::shared_ptr<CleaningRecordStep> last() const;
    uint32_t size() const;
    const std::shared_ptr<CleaningRecordStep> operator[](std::size_t idx);
    uint32_t getMaxSteps() const { return MaxSteps; }
    std::string getAlgorithmName() const { return algorithmName; }
    friend std::ostream& operator<<(std::ostream& os, const CleaningRecord& record);

private:
    bool hasInitialStep;
    uint32_t MaxSteps;
    std::string algorithmName;
    std::vector<std::shared_ptr<CleaningRecordStep>> steps;
};
