#pragma once
#include <vector>
#include <filesystem>
#include <string>

class SimulationArguments {
public:
    SimulationArguments(int argc, char** argv);
    bool isHelp() const;
    bool isSummaryOnly() const { return summaryOnly; }
    const std::vector<std::filesystem::path> & getHouseFiles() const { return houseFiles; }
    const std::vector<std::filesystem::path> & getAlgorithmFiles() const { return algoFiles; }
    uint32_t getNumThreads() const { return numThreads; }
private:
    bool hasFlag(const std::string& flag) const;
    bool isValidDirectory(const std::string& pathStr);
    bool summaryOnly = false;
    std::vector<std::filesystem::path> houseFiles;
    std::vector<std::filesystem::path> algoFiles;
    uint32_t numThreads = 10;


};