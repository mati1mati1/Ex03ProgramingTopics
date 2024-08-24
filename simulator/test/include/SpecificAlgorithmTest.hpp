#include "gtest/gtest.h"
#include "VacuumSimulator.hpp"
#include <filesystem>
#include <dlfcn.h>
#include "AlgorithmRegistrar.h"
class SpecificAlgorithmTest : public ::testing::Test
{
protected:
    virtual void StartTest(std::filesystem::path inputfile,std::filesystem::path algorithmPath)
    {
        filename = inputfile.stem().string();
        void *handle = dlopen(algorithmPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if (!handle)
        {
            FAIL() << "Failed to open the algorithm file " << dlerror();
        }
        algoName = algorithmPath.stem().string();
        auto algorithm = AlgorithmRegistrar::getAlgorithmRegistrar().begin()->create();
        simulator.setAlgorithm(std::move(algorithm));
        simulator.readHouseFile(inputfile);
        simulator.run();
        record = simulator.record;
        ASSERT_EQ(record->last()->getLocationType(), LocationType::CHARGING_STATION);
        auto indecies = record->size();
        ASSERT_EQ((*record)[indecies].get()->getStep(), Step::Finish);
        if (record->size() != 0)
        {
            ASSERT_NE((*record)[indecies - 1].get()->getStep(), Step::Stay);
        }
        std::filesystem::create_directories(gt);
    }
    void TearDown() override
    {
        if (!testing::Test::HasFailure())
        {
            auto path = simulator.exportRecord(algoName);
            auto gtPath = gt / (filename + "-" + algoName + ".txt");
            std::filesystem::copy(path, gtPath, std::filesystem::copy_options::overwrite_existing);
        }
    }
    VacuumSimulator simulator;
    std::string filename;
    std::string algoName;
    void* handle;
    std::shared_ptr<CleaningRecord> record;
    std::filesystem::path gt= "../test/examples/gt";
};
