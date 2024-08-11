#include "AbstractAlgorithm.h"
#include "SimulationArguments.hpp"
#include "AlgorithmRegistrar.h"
#include "VacuumSimulator.hpp"
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

std::mutex summaryMutex;
const std::filesystem::path CWD = std::filesystem::current_path();
void runSimulation(const std::string name, std::unique_ptr<AbstractAlgorithm> algorithm, const std::filesystem::path& houseFile, SimulationArguments& args, char *threadStatus)
{
    VacuumSimulator simulator;
    boost::asio::io_context context;
    simulator.readHouseFile(houseFile);
    auto maxTime = simulator.getMaxTime();
    boost::asio::steady_timer timer(context, maxTime);

    std::future<void> simFuture = std::async(std::launch::async, [&]() {
        try {
           
            simulator.setAlgorithm(std::move(algorithm));
            simulator.run(name);
            {
                std::lock_guard<std::mutex> lock(summaryMutex);
                if (!args.isSummaryOnly())
                {
                    simulator.exportRecord();
                }
                simulator.exportSummary();
            }
            *threadStatus = 1;
            context.stop();
        }
        catch (const std::invalid_argument& e) {
            std::ofstream errorFile(CWD / (houseFile.stem().string() + ".error"));
            errorFile << "Error: Unable to parse House file: " << houseFile.stem().string() << e.what() << std::endl;
            errorFile.close();
            context.stop();
            *threadStatus = 1;
        }
        catch (const std::exception& e) {
            std::ofstream errorFile(CWD / (houseFile.stem().string() + ".error"));
            errorFile << "Error: Simulator Error " << houseFile.stem().string() << e.what() << std::endl;
            errorFile.close();
            context.stop();
            *threadStatus = 1;
        }
    });

    timer.async_wait([&](const boost::system::error_code& ec) {
        if (!ec) {
            simFuture.wait_for(std::chrono::seconds(0));
            simFuture = {}; // effectively cancels the task
            *threadStatus = 1;
            context.stop();
        }
    });

    context.run();

    if (simFuture.valid())
    {
        /*
            Thorws any expection that may have occured
        */
        simFuture.get(); 
    }
}

void reserveHandles(const std::vector<std::filesystem::path> & algorithmFiles, std::vector<void *> &Outhandles)
{
    for (const auto &algoFile : algorithmFiles)
    {
        void *handle = dlopen(algoFile.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        std::string algoName = algoFile.stem().string();
        if (!handle)
        {
            std::cerr << "Failed to load algorithm file: " << algoFile << std::endl;
            std::ofstream errorFile(CWD / (algoName + ".error"));
            errorFile << "Error: Unable to load algorithm file: " << dlerror() << std::endl;
            errorFile.close();
            continue;
        }
        Outhandles.emplace_back(handle);
    }
}
void clearHandles(std::vector<void *> &handles)
{
    for (const auto &handle : handles)
    {
        dlclose(handle);
    }
}
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
int main(int argc, char **argv)
{
    try
    {
        SimulationArguments args(argc, argv);
        std::vector<void *> handles;
        reserveHandles(args.getAlgorithmFiles(), handles);
        auto numThreads = args.getNumThreads();
        std::vector<std::unique_ptr<Task>> tasks;
        std::vector<char> threadStatuses(numThreads , 0);
        auto &algorithms = AlgorithmRegistrar::getAlgorithmRegistrar();
        auto houseFiles = args.getHouseFiles();

        auto algorithm = algorithms.begin();
        auto houseFile = houseFiles.begin();    
        auto houseFileBegin = houseFiles.begin();
        
        while (algorithm != algorithms.end() && houseFile != houseFiles.end())
        {
            if (tasks.size() >= numThreads)
            {
                for (auto task = tasks.begin(); task != tasks.end();)
                {
                    if ((*task)->getMonitor() != 0 && (*task)->getThread().joinable())
                    {
                        (*task)->getThread().join();
                        (*task)->resetMonitor();
                        task = tasks.erase(task);
                    }
                    else
                    {
                        ++task;
                    }
                }
            }
            if (tasks.size() >= numThreads)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            const std::string& name = algorithm->name();
            std::unique_ptr<AbstractAlgorithm> algorithmInstance = algorithm->create();
            char* monitor = &threadStatuses.at(tasks.size());
            tasks.emplace_back(std::make_unique<Task>(std::thread(runSimulation, name, std::move(algorithmInstance), *houseFile, std::ref(args), monitor),monitor));
            
            if (++houseFile == houseFiles.end())
            {
                houseFile = houseFileBegin;
                ++algorithm;
            }
        }

        for (auto &task : tasks)
        {
            if (task->getThread().joinable())
            {
                task->getThread().join();
            }
        }

        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        clearHandles(handles);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}

