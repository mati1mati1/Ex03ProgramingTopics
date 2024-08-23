#pragma once
#include "SimulationArguments.hpp"
#include <thread>
#include <utility>
#include <condition_variable>
class BatchVacuumSimulator
{
    public:
        void run(const SimulationArguments &args);
        inline static const std::filesystem::path CWD = std::filesystem::current_path();
        ~BatchVacuumSimulator();
    private:
        void reserveHandles(const std::vector<std::filesystem::path> & algorithmFiles);
        void clearHandles();
    private:
        std::mutex summaryMutex;
        std::vector<void *> handles;

};