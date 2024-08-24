#include "SpecificAlgorithmTest.hpp"

class BFSSimultaneousMappingAndCleaningAlgorithmTest : public SpecificAlgorithmTest{
public:
    void StartTest(std::filesystem::path inputfile)
    {
        SpecificAlgorithmTest::StartTest(inputfile, "../../badAndGoodLib/libAlgo_323012971_315441972_Simultaneous.so");
    }
};
class FutileTest : public BFSSimultaneousMappingAndCleaningAlgorithmTest
{
};
class MappingTest : public BFSSimultaneousMappingAndCleaningAlgorithmTest
{
};
class CleaningTest : public BFSSimultaneousMappingAndCleaningAlgorithmTest
{
};

TEST_F(FutileTest, minHouse) {
    StartTest("../../simulator/test/examples/futileTest/house-minvalid.house");
    ASSERT_EQ(record->size(), 0);

}
TEST_F(FutileTest, tooLittleBatteryToDoAnythingUseful){
    StartTest("../../simulator/test/examples/futileTest/house-return-small-battery.house");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(FutileTest, houseLockedinWithDirt){
    StartTest("../../simulator/test/examples/futileTest/house-closedin2.house");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(FutileTest, houseMax0){
    StartTest("../../simulator/test/examples/futileTest/house-max0.house");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(FutileTest, closedInHouse) {
    StartTest("../../simulator/test/examples/futileTest/house-closedin.house");
    ASSERT_EQ(record->size(), 0);
}
TEST_F(MappingTest, lineHouseSomeUnmappable){
    StartTest("../../simulator/test/examples/mappingTest/house-linemappable-not-cleanable.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 1000);
    ASSERT_EQ(record->last()->getDirtLevel(), 9);

}

TEST_F(MappingTest, cleanHouse) {
    StartTest("../../simulator/test/examples/futileTest/house-nodirt.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100);
}
TEST_F(MappingTest, lineHouse) {
    StartTest("../../simulator/test/examples/futileTest/house-line.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 1000);
    ASSERT_EQ(record->last()->getDirtLevel(), 9);
}

TEST_F(MappingTest, bigEmptyWithReturningToChargeSomeUnmappble){
    StartTest("../../simulator/test/examples/mappingTest/house-bigEmpty-someUnmappable.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(MappingTest, bigEmptyWithReturningToChargeAllDiscoverable){
    StartTest("../../simulator/test/examples/mappingTest/house-bigEmpty.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(MappingTest, bigEmptyWithReturningToChargeSomeUnDiscoverable){
    StartTest("../../simulator/test/examples/mappingTest/house-bigEmpty-allFoundButSomeUndiscoverable.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 100000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, house){
    StartTest("../../simulator/test/examples/cleaningTest/house.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_EQ(record->size(), 10);
    ASSERT_EQ((*record)[0]->getDirtLevel(),26);
    ASSERT_LT(record->last()->getDirtLevel(), 26);
}
TEST_F(CleaningTest, houseCorridors){
    StartTest("../../simulator/test/examples/cleaningTest/house-coridors.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 100);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, houseCorridorsWithEmptyRows){
    StartTest("../../simulator/test/examples/cleaningTest/house-empty-rows-counted-as-corridors.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 11);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, houseMaxSteps){
    StartTest("../../simulator/test/examples/cleaningTest/house-maxsteps.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_EQ(record->size(), 5);
    ASSERT_EQ(record->last()->getDirtLevel(), 6);
}
TEST_F(CleaningTest, houseExatStepsAsMaxTest){
    StartTest("../../simulator/test/examples/cleaningTest/house-partial-exact-steps.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LE(record->size(), 32);
    ASSERT_EQ(record->last()->getDirtLevel(), 17);
}
TEST_F(CleaningTest, littleBattery){
    StartTest("../../simulator/test/examples/cleaningTest/house-return-small-battery2.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 200);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}

TEST_F(CleaningTest, narrowHouse){
    StartTest("../../simulator/test/examples/cleaningTest/house-narrow.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 25);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, narrowHouseExactBattery){
    StartTest("../../simulator/test/examples/cleaningTest/house-narrow-exact-battery-andsteps.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 25);
    ASSERT_LT(record->last()->getDirtLevel(), 4);
}
TEST_F(CleaningTest, narrowHouseExactSteps){
    StartTest("../../simulator/test/examples/cleaningTest/house-narrow-exact-battery.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 25);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, housePartial){
    StartTest("../../simulator/test/examples/cleaningTest/house-partial.house");
    ASSERT_NE(record->size(), 0);
    ASSERT_NE(record->size(), 1000);
    ASSERT_GT((*record)[0]->getDirtLevel(),17);
    ASSERT_EQ(record->last()->getDirtLevel(), 17);
}
TEST_F(CleaningTest, houseSparse){
    StartTest("../../simulator/test/examples/cleaningTest/house-sparse.house");
    ASSERT_GT((*record)[0]->getDirtLevel(),100);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
}
TEST_F(CleaningTest, houseSparse2){
    StartTest("../../simulator/test/examples/cleaningTest/house-sparse2.house");
    ASSERT_GT((*record)[0]->getDirtLevel(),10);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 2500);
}
TEST_F(CleaningTest, houseBig){
    StartTest("../../simulator/test/examples/cleaningTest/house-big.house");
    ASSERT_GT((*record)[0]->getDirtLevel(),200);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 10000);
    std::cout << record->size() << std::endl;
}
TEST_F(CleaningTest, houseBig1){
    StartTest("../../simulator/test/examples/cleaningTest/house-big-1.house");
    ASSERT_GT((*record)[0]->getDirtLevel(),1000);
    ASSERT_EQ(record->last()->getDirtLevel(), 0);
    ASSERT_NE(record->size(), 0);
    ASSERT_LT(record->size(), 7000);
}
