#include "VacuumSimulator.hpp"
#include "AlgorithmRegistrar.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <filesystem>
#include <dlfcn.h>
#include <fstream>
#include <algorithm>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    std::string housePath;
    std::string algoPath;

    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")("house_path", po::value<std::string>(&housePath), "set house files path")("algo_path", po::value<std::string>(&algoPath), "set algorithm files path");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (const po::error &e)
    {
        std::cerr << "Error parsing command-line arguments: " << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    if (housePath.empty())
    {
        housePath = fs::current_path().string();
    }

    if (algoPath.empty())
    {
        algoPath = fs::current_path().string();
    }
    std::vector<fs::path> houseFiles;
    for (const auto &entry : fs::directory_iterator(housePath))
    {
        if (entry.path().extension() == ".house")
        {
            houseFiles.push_back(entry.path());
        }
    }
    std::vector<fs::path> algoFiles;
    for (const auto &entry : fs::directory_iterator(algoPath))
    {
        if (entry.path().extension() == ".so")
        {
            algoFiles.push_back(entry.path());
        }
    }

    if (houseFiles.empty())
    {
        std::cerr << "No .house files found in directory: " << housePath << std::endl;
        return 1;
    }

    if (algoFiles.empty())
    {
        std::cerr << "No .so files found in directory: " << algoPath << std::endl;
        return 1;
    }
    std::vector<void *> handles;
    for (const auto &algoFile : algoFiles)
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
        handles.emplace_back(handle);
    }
    auto &algorithms = AlgorithmRegistrar::getAlgorithmRegistrar();
    for (const auto &algorithm: algorithms)
    {
        VacuumSimulator simulator;
        for (const auto &houseFile : houseFiles)
        {
            try
            {
                simulator.readHouseFile(houseFile);
                auto algorithmInstance = algorithm.create();
                simulator.setAlgorithm(std::move(algorithmInstance));
                simulator.run(algorithm.name());
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
    for (const auto &handle : handles)
    {
        dlclose(handle);
    }
    return 0;
}
