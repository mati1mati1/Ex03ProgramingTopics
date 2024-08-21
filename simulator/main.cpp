#include "BatchVacuumSimulator.hpp"
#include "SimulationArguments.hpp"
#include <iostream>


int main(int argc, char **argv)
{
    try
    {
        SimulationArguments args(argc, argv);
        BatchVacuumSimulator simulator;
        simulator.run(args);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

