#include "CleaningRecordStep.hpp"
#include "Step.hpp"
#include <sstream>

std::ostream& operator<<(std::ostream& os, const CleaningRecordStep& record) {
    os << record.getStep();
    return os;
} 