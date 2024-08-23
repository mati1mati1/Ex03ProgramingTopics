#include <stdexcept>

extern "C" void* create_algorithm() {
    throw std::runtime_error("Bad algorithm: This algorithm is supposed to fail.");
    return nullptr;
}
