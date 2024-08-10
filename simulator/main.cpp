#include "VacuumSimulator.hpp"
#include "AlgorithmRegistrar.h"
#include <iostream>
#include <filesystem>
#include <dlfcn.h>
#include <fstream>
#include "SimulationArguments.hpp"
void reserveHandles(const std::vector<std::filesystem::path> & algorithmFiles, std::vector<void *> &Outhandles);
void clearHandles(std::vector<void *> &handles);
int main(int argc, char **argv)
{
    try
    {
        SimulationArguments args(argc, argv);
        std::vector<void *> handles;
        reserveHandles(args.getAlgorithmFiles(), handles);
        
        auto &algorithms = AlgorithmRegistrar::getAlgorithmRegistrar();
        for (const auto &algorithm: algorithms)
        {
            VacuumSimulator simulator;
            for (const auto &houseFile : args.getHouseFiles())
            {
                try
                {
                    simulator.readHouseFile(houseFile);
                    auto algorithmInstance = algorithm.create();
                    simulator.setAlgorithm(std::move(algorithmInstance));
                    simulator.run(algorithm.name());
                    simulator.exportRecord();
                    simulator.exportSummary();
                }
                catch(const std::invalid_argument& e)
                {
                    std::ofstream errorFile(houseFile.stem().string() + ".error");
                    errorFile << "Error: Unable to parse House file: " <<houseFile.stem().string()<< std::endl;
                    errorFile.close();
                }
                catch(const std::exception& e)
                {
                    std::ofstream errorFile(houseFile.stem().string() + ".error");
                    errorFile << "Error: Simulator Error " << houseFile.stem().string() << std::endl;
                    errorFile.close();
                }
                
                
            }
        }
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        clearHandles(handles);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
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
            std::ofstream errorFile(algoName + ".error");
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