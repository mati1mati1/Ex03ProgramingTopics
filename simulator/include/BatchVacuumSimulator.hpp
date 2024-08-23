#pragma once
#include "SimulationArguments.hpp"
#include <thread>
#include <utility>
#include <condition_variable>
class Task
{
    public:
        Task(std::thread &&thread,char* monitor) : threadfd(std::move(thread)),isDone(monitor) {}
        char getMonitor() { return *isDone; }
        void resetMonitor() { if (isDone != nullptr) {*isDone = 0;} }
        auto& getThread() { return threadfd; }
        ~Task() {
            if(threadfd.joinable())
            {
                threadfd.join(); 
            }
            resetMonitor();
        }
    private:
        std::thread threadfd;
        char* isDone;

};
class BatchVacuumSimulator
{
    public:
        void run(const SimulationArguments &args);
        inline static const std::filesystem::path CWD = std::filesystem::current_path();
        ~BatchVacuumSimulator();
    private:
        void waitForSpot(uint32_t &numThreads,std::vector<std::unique_ptr<Task>> &tasks);
        void waitAllTasks();
        void reapTasks(std::vector<std::unique_ptr<Task>> &tasks);
        void clearRun();
        void reserveHandles(const std::vector<std::filesystem::path> & algorithmFiles);
        void enqueueTask(const SimulationArguments &args, const std::filesystem::path &houseFile, auto &algorithm);
        void clearHandles();
    private:
        std::vector<std::unique_ptr<Task>> tasks;
        std::vector<char> threadStatuses;
        std::mutex summaryMutex;
        std::condition_variable queueNotifier;
        std::vector<void *> handles;


};