#include "VacuumSimulator.hpp"
#include "CleaningRecord.hpp"
#include "CleaningRecordStep.hpp"
#include "VacuumPayload.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

class VacuumSimulatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        CleaningRecordStep initialStep = CleaningRecordStep(LocationType::CHARGING_STATION, Step::North, 5, 10);
        record = std::make_shared<CleaningRecord>(initialStep, 10);
        std::filesystem::create_directory(testOutputDir);
        simulator = std::make_unique<VacuumSimulator>();
    }

    void TearDown() override {
        std::filesystem::remove_all(testOutputDir);
    }
    void writeOutFile(std::ofstream &writeStream){
        simulator->record = record;
        simulator->writeOutFile(writeStream);
    }

    std::unique_ptr<VacuumSimulator> simulator;
    std::shared_ptr<CleaningRecord> record;
    std::filesystem::path testOutputDir = "/tmp/test_output";
};

TEST_F(VacuumSimulatorTest, WORKING) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North, 4, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 9));
    
    std::filesystem::path resultPath = testOutputDir / "VacuumSimulatorTest-working.txt";
    std::ofstream writeStream(resultPath);
    if (!writeStream.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;    
        throw std::runtime_error("Unable to open file.");
    }
    writeOutFile(writeStream);
    
    std::ifstream outFileRead(resultPath);
    ASSERT_TRUE(outFileRead.is_open());

    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 4");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 9");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = WORKING");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = FALSE");

    uint32_t score = 4 + 9 * 300 + 1000;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NEsW");

    outFileRead.close();
}

TEST_F(VacuumSimulatorTest, DEAD) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North, 4, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North, 0, 9));

    std::filesystem::path resultPath = testOutputDir / "VacuumSimulatorTest-dead.txt";

    std::ofstream writeStream(resultPath);
    if (!writeStream.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;    
        throw std::runtime_error("Unable to open file.");
    }
    writeOutFile(writeStream);

    ASSERT_TRUE(std::filesystem::exists(resultPath));
    
    std::ifstream outFileRead(resultPath);
    ASSERT_TRUE(outFileRead.is_open());

    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 5");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 9");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = DEAD");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = FALSE");

    uint32_t score = 10 + 9 * 300 + 2000;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NEsWN");

    outFileRead.close();
}

TEST_F(VacuumSimulatorTest, WithoutStep) {
    std::filesystem::path resultPath = testOutputDir / "VacuumSimulatorTest-withoutstep.txt";
    std::ofstream writeStream(resultPath);
    if (!writeStream.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;    
        throw std::runtime_error("Unable to open file.");
    }
    writeOutFile(writeStream);
    ASSERT_TRUE(std::filesystem::exists(resultPath));
    
    std::ifstream outFileRead(resultPath);
    ASSERT_TRUE(outFileRead.is_open());

    uint32_t score = 0 + 10 * 300 + 0;
    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 0");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 10");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = WORKING");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = TRUE");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "");

    outFileRead.close();
}

TEST_F(VacuumSimulatorTest, FINISHED) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North, 4, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 7));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 6));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 4));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 2));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 1));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 0));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 0));
    record->add(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Finish, 0, 0));

    std::filesystem::path resultPath = testOutputDir / "VacuumSimulatorTest-finished.txt";

    std::ofstream writeStream(resultPath);
    if (!writeStream.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;    
        throw std::runtime_error("Unable to open file.");
    }
    writeOutFile(writeStream);
    ASSERT_TRUE(std::filesystem::exists(resultPath));
    
    std::ifstream outFileRead(resultPath);
    ASSERT_TRUE(outFileRead.is_open());

    uint32_t score = 12 + 0 * 300 + 0;
    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 12");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 0");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = FINISHED");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = TRUE");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NEsWEssssssWF");

    outFileRead.close();
}
TEST_F(VacuumSimulatorTest, TIMEOUT) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North, 4, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 7));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 6));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 4));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 2));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 1));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 0));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 0));
    record->add(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Finish, 0, 0));

    std::filesystem::path resultPath = testOutputDir / "VacuumSimulatorTest-timeout.txt";
    std::ofstream writeStream(resultPath);
    if (!writeStream.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;    
        throw std::runtime_error("Unable to open file.");
    }
    simulator->timeout();
    writeOutFile(writeStream);
    ASSERT_TRUE(std::filesystem::exists(resultPath));
    
    std::ifstream outFileRead(resultPath);
    ASSERT_TRUE(outFileRead.is_open());

    uint32_t score = 10 * 2 + 10 * 300 + 2000 ;
    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 12");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 0");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = FINISHED");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = TRUE");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NEsWEssssssWF");

    outFileRead.close();
}
