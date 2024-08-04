#include <iostream>
#include "Logger.hpp"
#include "MappingAlgorithm.hpp"
#include "VacuumSimulator.hpp"
bool getRand(int argc, char **argv);

int main(int argc, char **argv)
{
    std::filesystem::path runPath ="/tmp/myrobot/application.log";
    Logger::getInstance().setLogFile(runPath);
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        Logger::getInstance().log("Error: Incorrect usage. Expected 1 argument, received " + std::to_string(argc - 1));
        return 1;
    }
    std::string inputFilePath = argv[1];
    auto config = std::make_shared<MappingAlgorithmConfig>();
    VacuumSimulator simulator;
    try {
        simulator.setAlgorithm(config);
        simulator.readHouseFile(inputFilePath);
        simulator.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::getInstance().log("Error: " + std::string(e.what()));
        return 1;
    }
    return 0;
}
