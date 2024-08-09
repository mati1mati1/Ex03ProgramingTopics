#include "gtest/gtest.h"
#include "VacuumSimulator.hpp"
#include "OutFileWriter.hpp"
#include <filesystem>
#include <dlfcn.h>
#include "AlgorithmRegistrar.h"
class SpecificAlgorithmTest : public ::testing::Test
{
protected:
    virtual void StartTest(std::filesystem::path inputfile,std::filesystem::path algorithmPath)
    {
        filename = inputfile.filename().string();
        VacuumSimulator simulator;
        void *handle = dlopen(algorithmPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if (!handle)
        {
            FAIL() << "Failed to open the algorithm file" << dlerror();
        }
        algoName = algorithmPath.stem().string();
        auto algorithm = AlgorithmRegistrar::getAlgorithmRegistrar().begin()->create();
        simulator.setAlgorithm(std::move(algorithm));
        simulator.readHouseFile(inputfile);
        record = simulator.calculate();
        ASSERT_EQ(record->last()->getLocationType(), LocationType::CHARGING_STATION);
        auto indecies = record->size();
        ASSERT_EQ((*record)[indecies].get()->getStep(), Step::Finish);
        if (record->size() != 0)
        {
            ASSERT_NE((*record)[indecies - 1].get()->getStep(), Step::Stay);
        }
    }
    void TearDown() override
    {
        if (record->size() != 0)
        {
            OutFileWriter writer;
            writer.write("../test/examples/gt/" + filename, record, algoName);
        }
    }
    std::string filename;
    std::string algoName;
    void* handle;
    std::shared_ptr<CleaningRecord> record;
};
