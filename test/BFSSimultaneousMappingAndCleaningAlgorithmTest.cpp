#include "gtest/gtest.h"
#include <filesystem>
#include <VacuumSimulator.hpp>
#include <MappingAlgorithm/BFSSimultaneousMappingAndCleaningAlgorithm.hpp>
#include "Logger.hpp"
#include <OutFileWriter.hpp>
class BFSSimultaneousMappingAndCleaningAlgorithmTest : public ::testing::Test {
    protected:
        void StartTest(std::filesystem::path inputfile, std::string filename)
        {
            Logger::getInstance().setLogFile("/tmp/myrobot/test.log");
            this->filename = filename;
            VacuumSimulator simulator;
            this->config = std::make_shared<BFSSimultaneousMappingAndCleaningAlgorithmConfig>();
            simulator.setAlgorithm(config);
            simulator.readHouseFile(inputfile);
            record = simulator.calculate();
            ASSERT_EQ(record->last()->getLocationType(), LocationType::CHARGING_STATION);
            auto indecies = record->size();
            ASSERT_EQ((*record)[indecies].get()->getStep(),Step::Finish);
            if(record->size() != 0)
            {
                ASSERT_NE((*record)[indecies-1].get()->getStep(),Step::Stay);
            }

        }
        void TearDown() override {
            if (record->size() != 0)
            {
                OutFileWriter writer;
                writer.write("../test/examples/gt/" + filename, record, config->getAlgorithmName());
            }
        }
        std::string filename;
        std::shared_ptr<CleaningRecord> record;
        std::shared_ptr<AlgorithmConfig> config;
};
class FutileTest : public BFSSimultaneousMappingAndCleaningAlgorithmTest {};
class MappingTest : public BFSSimultaneousMappingAndCleaningAlgorithmTest {};
class CleaningTest : public BFSSimultaneousMappingAndCleaningAlgorithmTest {};

TEST_F(FutileTest, minHouse) {
    StartTest("../test/examples/futileTest/house-minvalid.txt", "house-minvalid.txt");
    ASSERT_EQ(record->size(), 0);

}
TEST_F(FutileTest, tooLittleBatteryToDoAnythingUseful){
    StartTest("../test/examples/futileTest/house-return-small-battery.txt", "house-return-small-battery.txt");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(FutileTest, houseLockedinWithDirt){
    StartTest("../test/examples/futileTest/house-closedin2.txt", "house-closedin2.txt");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(FutileTest, houseMax0){
    StartTest("../test/examples/futileTest/house-max0.txt", "house-max0.txt");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(FutileTest, closedInHouse) {
    StartTest("../test/examples/futileTest/house-closedin.txt", "house-closedin.txt");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(MappingTest, lineHouseSomeUnmappable){
    StartTest("../test/examples/mappingTest/house-linemappable-not-cleanable.txt", "house-linemappable-not-cleanable.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 1000);
    ASSERT_EQ(record->last()->getDirtLevel(), 9);

}

TEST_F(MappingTest, cleanHouse) {
    StartTest("../test/examples/futileTest/house-nodirt.txt", "house-nodirt.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100);
}
TEST_F(MappingTest, lineHouse) {
    StartTest("../test/examples/futileTest/house-line.txt", "house-line.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 1000);
    ASSERT_EQ(record->last()->getDirtLevel(), 9);
}

TEST_F(MappingTest, bigEmptyWithReturningToChargeSomeUnmappble){
    StartTest("../test/examples/mappingTest/house-bigEmpty-someUnmappable.txt", "house-bigEmpty-someUnmappable.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(MappingTest, bigEmptyWithReturningToChargeAllDiscoverable){
    StartTest("../test/examples/mappingTest/house-bigEmpty.txt", "house-bigEmpty.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(MappingTest, bigEmptyWithReturningToChargeSomeUnDiscoverable){
    StartTest("../test/examples/mappingTest/house-bigEmpty-allFoundButSomeUndiscoverable.txt", "house-bigEmpty-allFoundButSomeUndiscoverable.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, house){
    StartTest("../test/examples/cleaningTest/house.txt", "house.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_EQ(record->size(), 10);
    ASSERT_EQ((*record)[0]->getDirtLevel(),26);
    ASSERT_LT(record->last()->getDirtLevel(), 26);
}
TEST_F(CleaningTest, houseCorridors){
    StartTest("../test/examples/cleaningTest/house-coridors.txt", "house-coridors.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 100);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, houseCorridorsWithEmptyRows){
    StartTest("../test/examples/cleaningTest/house-empty-rows-counted-as-corridors.txt", "house-empty-rows-counted-as-corridors.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 11);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, houseMaxSteps){
    StartTest("../test/examples/cleaningTest/house-maxsteps.txt" , "house-maxsteps.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_EQ(record->size(), 5);
    ASSERT_EQ(record->last()->getDirtLevel(), 6);
}
TEST_F(CleaningTest, houseExatStepsAsMaxTest){
    StartTest("../test/examples/cleaningTest/house-partial-exact-steps.txt" , "house-partial-exact-steps.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LE(record->size(), 32);
    ASSERT_EQ(record->last()->getDirtLevel(), 17);
}
TEST_F(CleaningTest, littleBattery){
    StartTest("../test/examples/cleaningTest/house-return-small-battery2.txt", "house-return-small-battery2.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 200);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}

TEST_F(CleaningTest, narrowHouse){
    StartTest("../test/examples/cleaningTest/house-narrow.txt", "house-narrow.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 25);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, narrowHouseExactBattery){
    StartTest("../test/examples/cleaningTest/house-narrow-exact-battery-andsteps.txt", "house-narrow.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 25);
    ASSERT_LT(record->last()->getDirtLevel(), 4);
}
TEST_F(CleaningTest, narrowHouseExactSteps){
    StartTest("../test/examples/cleaningTest/house-narrow-exact-battery.txt", "house-narrow.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 25);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, housePartial){
    StartTest("../test/examples/cleaningTest/house-partial.txt", "house-partial.txt");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 1000);
    ASSERT_GT((*record)[0]->getDirtLevel(),17);
    ASSERT_EQ(record->last()->getDirtLevel(), 17);
}
TEST_F(CleaningTest, houseSparse){
    StartTest("../test/examples/cleaningTest/house-sparse.txt", "house-sparse.txt");
    ASSERT_GT((*record)[0]->getDirtLevel(),100);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, houseSparse2){
    StartTest("../test/examples/cleaningTest/house-sparse2.txt", "house-sparse2.txt");
    ASSERT_GT((*record)[0]->getDirtLevel(),10);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 2500);
}
TEST_F(CleaningTest, houseBig){
    StartTest("../test/examples/cleaningTest/house-big.txt", "house-big.txt");
    ASSERT_GT((*record)[0]->getDirtLevel(),200);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 10000);
    std::cout << record->size() << std::endl;
}
TEST_F(CleaningTest, houseBig1){
    StartTest("../test/examples/cleaningTest/house-big-1.txt",  "house-big-1.txt");
    ASSERT_GT((*record)[0]->getDirtLevel(),1000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 7000);
}