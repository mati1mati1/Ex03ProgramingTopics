#include "BatchVacuumSimulator.hpp"
#include "AbstractAlgorithm.h"
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
#include <dlfcn.h>
#include <queue>

void writeErrorFile(const std::string& algorithmName, const std::string& errorMessage);
std::filesystem::path getErrorPathFile(const std::filesystem::path& houseFile, const std::string& algorithmName ) {
    constexpr std::string_view errorExtension = ".error";
    if (houseFile.empty() && algorithmName.empty())
    {
        throw std::invalid_argument("Could not create error file");
    }
    if (algorithmName.empty())
    {
        return  BatchVacuumSimulator::CWD /(houseFile.stem().string() + errorExtension.data());
    }
    if(houseFile.empty())
    {
        return  BatchVacuumSimulator::CWD /(algorithmName + errorExtension.data());
    }
    throw std::invalid_argument("Could not create error file");
}
void writeErrorFile(const std::filesystem::path& houseFile,const std::string& algorithmName, const std::string& errorMessage) {

    auto path = getErrorPathFile(houseFile, algorithmName);
    if (!(path.has_filename() && path.has_extension() && std::filesystem::exists(path.parent_path())))
    {
        throw std::invalid_argument("Could not create error file");
    }
    std::ofstream errorFile(path);
    if (!errorFile.is_open())
    {
        throw std::invalid_argument("Could not open error file");
    }
    errorFile << errorMessage << std::endl;
    errorFile.close();
}
void writeErrorFile(const std::string& algorithmName, const std::string& errorMessage)
{
    writeErrorFile("",algorithmName,errorMessage);
}
void writeErrorFile(const std::filesystem::path& houseFile, const std::string& errorMessage)
{
    writeErrorFile(houseFile,"",errorMessage);
}


void runSimulation(const std::string name, std::unique_ptr<AbstractAlgorithm> algorithm, const std::filesystem::path& houseFile,
    const SimulationArguments& args,std::mutex &summaryMutex)
{
    VacuumSimulator simulator;
    boost::asio::io_context context;
    try
    {
        simulator.readHouseFile(houseFile);
    }
    catch(const std::exception& e)
    {
        std::string errorMessage = "Error: Unable to read House file: " + houseFile.stem().string() + e.what();
        writeErrorFile(houseFile, errorMessage);
        return;
    }
    auto maxTime = simulator.getMaxTime();
    boost::asio::steady_timer timer(context, maxTime);
    bool isTimedOut = false;
    std::future<void> simFuture = std::async(std::launch::async, [&]() {
        try {
           
            simulator.setAlgorithm(std::move(algorithm));
            simulator.run(name);
            context.stop();
        }
        catch (const std::invalid_argument& e) {
            std::string errorMessage = "Error: Unable to parse House file: " + houseFile.stem().string() + e.what();
            writeErrorFile(houseFile, errorMessage);
            context.stop();
        }
        catch (const std::exception& e) {
            std::string errorMessage = "Error: Simulator Error " + houseFile.stem().string() + e.what();
            writeErrorFile(houseFile, errorMessage);
            context.stop();
        }
    });

    timer.async_wait([&](const boost::system::error_code& ec) {
        if (!ec) {
            simFuture.wait_for(std::chrono::seconds(0));
            simFuture = {}; // effectively cancels the task
            context.stop();
            isTimedOut = true;
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
    try
    {
        {
            std::lock_guard<std::mutex> lock(summaryMutex);
            if (!args.isSummaryOnly())
            {
                simulator.exportRecord(isTimedOut);
            }
            simulator.exportSummary(isTimedOut);
        }
        
    } 
    catch (const std::exception& e)
    {
        std::string errorMessage = "Error: Unable to write output file: " + houseFile.stem().string() + e.what();
        writeErrorFile(houseFile, errorMessage);
    }
}


void BatchVacuumSimulator::reserveHandles(const std::vector<std::filesystem::path> & algorithmFiles)
{
    for (const auto &algoFile : algorithmFiles)
    {
        auto preOpenCount = AlgorithmRegistrar::getAlgorithmRegistrar().count();
        void *handle = dlopen(algoFile.c_str(), RTLD_LAZY);
        std::string algoName = algoFile.stem().string();
        if (!handle)
        {
            std::string errorMessage = "Error: Unable to load algorithm file: " + std::string(dlerror());
            writeErrorFile(algoName, errorMessage);
            continue;
        }
        auto postOpenCount = AlgorithmRegistrar::getAlgorithmRegistrar().count();
        if(postOpenCount != preOpenCount + 1)
        {
            std::string errorMessage = "Error: Unable to load algorithm file: " + algoName + " - No algorithm was registered / Multiple algorithms were registered";
            writeErrorFile(algoName, errorMessage);
            dlclose(handle);
            continue;
        }
        handles.emplace_back(handle);
    }
}
void BatchVacuumSimulator::clearHandles()
{
    for (auto &handle : handles)
    {
        if (handle != nullptr)
        {
            dlclose(handle);
        }
        
        handle = nullptr;
    }
    handles.clear();
}
BatchVacuumSimulator::~BatchVacuumSimulator()
{
    clearHandles();
}


class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> job);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void workerThread();
};

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] { workerThread(); });
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        jobs.push(std::move(job));
    }
    condition.notify_one();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !jobs.empty(); });
            if (stop && jobs.empty()) {
                return;
            }
            job = std::move(jobs.front());
            jobs.pop();
        }
        job();
    }
}

void BatchVacuumSimulator::run(const SimulationArguments &args) {
    reserveHandles(args.getAlgorithmFiles());
    //auto numThreads = args.getNumThreads();
    //ThreadPool threadPool(numThreads);
    
    auto algorithms = AlgorithmRegistrar::getAlgorithmRegistrar();
   // auto houseFiles = args.getHouseFiles();
    
    /*for (auto algorithm = algorithms.begin(); algorithm != algorithms.end(); ++algorithm) {
        for (auto houseFile = houseFiles.begin(); houseFile != houseFiles.end(); ++houseFile) {
            threadPool.enqueue([&]() {
                // std::unique_ptr<AbstractAlgorithm> algorithmInstance = nullptr;
                // std::string name;
                // try {
                //     name = algo->name();
                //     algorithmInstance = algo->create();
                //     runSimulation(name, std::move(algorithmInstance), house, args, std::ref(summaryMutex));
                // } catch (const std::exception &e) {
                //     writeErrorFile(house, e.what());
                // }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            });
        }
    }*/
    clearHandles();
}