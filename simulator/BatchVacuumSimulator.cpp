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

class factoryException : public std::exception
{
    public:
        factoryException(const std::string& message) : message(message) {}
        const char* what() const noexcept override
        {
            return message.c_str();
        }
    private:
        std::string message;
};
void writeErrorFile(const std::filesystem::path& houseFile,const std::string& algorithmName, const std::string& errorMessage);
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

void runSimulation(const std::string name, std::unique_ptr<AbstractAlgorithm> algorithm, const std::filesystem::path& houseFile,const SimulationArguments& args,std::mutex &summaryMutex, std::shared_ptr<std::counting_semaphore<>> semaphore)
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
        semaphore->release();
        return;
    }
    auto maxTime = simulator.getMaxTime();
    boost::asio::steady_timer timer(context, maxTime);
    std::atomic<bool> isTimedOut = false;
    std::atomic<bool> error = false;
    std::string errorMessage;
    std::future<void> simFuture = std::async(std::launch::async, [&]() {
        try {
            simulator.setAlgorithm(std::move(algorithm));
            simulator.run();
            context.stop();
            return;
        }
        catch (const std::invalid_argument& e) {
            errorMessage = "Error: Unable to parse House file: " + houseFile.stem().string() + e.what();
            error = true;
        }
        catch (const std::exception& e) {
            errorMessage = "Error: Simulator Error " + houseFile.stem().string() + e.what();
            error = true;
        }
        if(!isTimedOut)
        {
            context.stop();
            writeErrorFile(houseFile, errorMessage);
        }
    });

    timer.async_wait([&](const boost::system::error_code& ec) {
        if (!ec) {
            if (simFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
                simulator.timeout();
                isTimedOut = true;
            }
            context.stop();
        }
    });

    context.run();

    if (!isTimedOut && simFuture.valid())
    {
        simFuture.get(); 
    }
    
    try
    {
        std::lock_guard<std::mutex> lock(summaryMutex);
        if (!error)
        {
            if (!args.isSummaryOnly())
            {
                simulator.exportRecord(name);
            }
        }
        simulator.exportSummary(name,error);
    } 
    catch (const std::exception& e)
    {
        std::string errorMessage = "Error: Unable to write output file: " + houseFile.stem().string() + e.what();
        writeErrorFile(houseFile, errorMessage);
    }
    
    
    semaphore->release();
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

void BatchVacuumSimulator::enqueueTask(const SimulationArguments &args, const std::filesystem::path &houseFile, auto &algorithm) {
    std::unique_ptr<AbstractAlgorithm> algorithmInstance = nullptr;
    std::string name;
    try{
        name = algorithm.name();
        algorithmInstance = algorithm.create();
    }catch(const std::exception& e)
    {
        throw factoryException("Error: Algorithm supplied is invalid factory of name cannot be resolved " + name + e.what());
    }
    threadPool.emplace_back(runSimulation, name, std::move(algorithmInstance),
                    houseFile, std::ref(args),std::ref(summaryMutex), semaphore);
}
void BatchVacuumSimulator::discardFinishedThreads()
{
    threadPool.erase(std::remove_if(threadPool.begin(), threadPool.end(), [](std::thread &thread) {
        if (thread.joinable())
        {
            return false;
        }
        return true;
    }), threadPool.end());
    
}

void BatchVacuumSimulator::run(const SimulationArguments &args) {
    reserveHandles(args.getAlgorithmFiles());
    auto numThreads = args.getNumThreads();
    auto &algorithms = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto houseFiles = args.getHouseFiles();
    semaphore = std::make_shared<std::counting_semaphore<>>(numThreads);
    auto algorithm = algorithms.begin();
    auto houseFile = houseFiles.begin();
    auto houseFileBegin = houseFiles.begin();
    
    while (algorithm != algorithms.end() && houseFile != houseFiles.end()) {
        semaphore->acquire();
        discardFinishedThreads();
        try{
            enqueueTask(args, *houseFile, *algorithm);
        }
        catch(const factoryException& e)
        {
            writeErrorFile(*houseFile, e.what());
            algorithm++;

        }
        if (++houseFile == houseFiles.end()) {
            houseFile = houseFileBegin;
            ++algorithm;
        }
    }
    for (auto &thread: threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threadPool.clear();
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    clearHandles();
}