#pragma once
#include "CleaningRecordStep.hpp"
#include <vector>
#include <memory>
#include <string>
enum class Status{
    DEAD,
    WORKING,
    FINISHED
};
inline std::ostream& operator<<(std::ostream& os, const Status& step) {
    switch (step) {
        case Status::DEAD: os << "DEAD"; break;
        case Status::WORKING: os << "WORKING"; break;
        case Status::FINISHED: os << "FINISHED"; break;
        default: throw std::invalid_argument("Invalid Status");
    }
    return os;
}
class CleaningRecord {
public:
    CleaningRecord(const CleaningRecordStep& initialStep, uint32_t maxSteps);
    void add(CleaningRecordStep step);
    const std::shared_ptr<CleaningRecordStep> getInitialStep() const;
    const std::shared_ptr<CleaningRecordStep> last() const;
    uint32_t size() const;
    const std::shared_ptr<CleaningRecordStep> operator[](std::size_t idx);
    uint32_t getMaxSteps() const { return maxSteps; }
    Status getStatus() const;
    friend std::ostream& operator<<(std::ostream& os, const CleaningRecord& record);
    uint32_t getInitialDirt();
    
private:
    bool hasInitialStep;
    uint32_t maxSteps;
    std::string algorithmName;
    std::vector<std::shared_ptr<CleaningRecordStep>> steps;
private:
    bool isDead() const { return size() != 0 && last()->getBatteryLevel() == 0 && !last()->isAtDockingStation(); } 
    bool isFinished() const { return last()->getStep() == Step::Finish;} 
    bool isAtMaxSteps() const { return size() == getMaxSteps(); } 

};
