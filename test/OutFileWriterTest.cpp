#include <gtest/gtest.h>
#include "OutFileWriter.hpp"
#include "CleaningRecord.hpp"
#include "CleaningRecordStep.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

class OutFileWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path logFilePath = "/tmp/test_logger.log";
        Logger::getInstance().setLogFile(logFilePath);

        CleaningRecordStep initialStep = CleaningRecordStep(LocationType::CHARGING_STATION, Step::North, 5, 10);
        record = std::make_shared<CleaningRecord>(initialStep);
    }

    void TearDown() override {
        std::filesystem::remove_all(testOutputDir);
    }

    std::shared_ptr<CleaningRecord> record;
    std::filesystem::path testOutputDir = "/tmp/test_output";
};

TEST_F(OutFileWriterTest, WORKING) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North,4, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 9));
    OutFileWriter writer;
    std::filesystem::path inputFilePath = testOutputDir / "input_house.txt";
    std::filesystem::create_directories(testOutputDir);

    std::filesystem::path outputFilePath = writer.write(inputFilePath, record);

    ASSERT_TRUE(std::filesystem::exists(outputFilePath));

    std::ifstream outFile(outputFilePath);
    ASSERT_TRUE(outFile.is_open());

    std::string line;
    std::getline(outFile, line);
    EXPECT_EQ(line, "NumSteps = 4");

    std::getline(outFile, line);
    EXPECT_EQ(line, "DirtLeft = 9");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Status = WORKING");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFile, line);
    EXPECT_EQ(line, "NEsW");

    outFile.close();
}

TEST_F(OutFileWriterTest, DEAD) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North,4, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::East, 3, 10));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::Stay, 2, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::West, 1, 9));
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North,0, 9));
    OutFileWriter writer;
    std::filesystem::path inputFilePath = testOutputDir / "input_house.txt";
    std::filesystem::create_directories(testOutputDir);

    std::filesystem::path outputFilePath = writer.write(inputFilePath, record);

    ASSERT_TRUE(std::filesystem::exists(outputFilePath));

    std::ifstream outFile(outputFilePath);
    ASSERT_TRUE(outFile.is_open());

    std::string line;
    std::getline(outFile, line);
    EXPECT_EQ(line, "NumSteps = 5");

    std::getline(outFile, line);
    EXPECT_EQ(line, "DirtLeft = 9");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Status = DEAD");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFile, line);
    EXPECT_EQ(line, "NEsWN");

    outFile.close();
}
TEST_F(OutFileWriterTest, WithoutStep) {
    OutFileWriter writer;
    std::filesystem::path inputFilePath = testOutputDir / "input_house.txt";
    std::filesystem::create_directories(testOutputDir);

    std::filesystem::path outputFilePath = writer.write(inputFilePath, record);

    ASSERT_TRUE(std::filesystem::exists(outputFilePath));

    std::ifstream outFile(outputFilePath);
    ASSERT_TRUE(outFile.is_open());

    std::string line;
    std::getline(outFile, line);
    EXPECT_EQ(line, "NumSteps = 0");

    std::getline(outFile, line);
    EXPECT_EQ(line, "DirtLeft = 10");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Status = WORKING");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFile, line);
    EXPECT_EQ(line, "");

    outFile.close();
}
TEST_F(OutFileWriterTest, FINISHED) {
    record->add(CleaningRecordStep(LocationType::HOUSE_TILE, Step::North,4, 10));
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
    record->add(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Finish,0, 0));
    OutFileWriter writer;
    std::filesystem::path inputFilePath = testOutputDir / "input_house.txt";
    std::filesystem::create_directories(testOutputDir);

    std::filesystem::path outputFilePath = writer.write(inputFilePath, record);

    ASSERT_TRUE(std::filesystem::exists(outputFilePath));

    std::ifstream outFile(outputFilePath);
    ASSERT_TRUE(outFile.is_open());

    std::string line;
    std::getline(outFile, line);
    EXPECT_EQ(line, "NumSteps = 12");

    std::getline(outFile, line);
    EXPECT_EQ(line, "DirtLeft = 0");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Status = FINISHED");

    std::getline(outFile, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFile, line);
    EXPECT_EQ(line, "NEsWEssssssWF");

    outFile.close();
}
TEST_F(OutFileWriterTest, HandleNoRecord) {
    OutFileWriter writer;
    std::filesystem::path inputFilePath = testOutputDir / "input_house.txt";

    std::filesystem::path outputFilePath = writer.write(inputFilePath, nullptr);

    ASSERT_TRUE(outputFilePath.empty());
}

TEST_F(OutFileWriterTest, CreateDirectoryIfNotExists) {
    OutFileWriter writer;
    std::filesystem::path newDir = testOutputDir / "new_dir";
    std::filesystem::path inputFilePath = newDir / "input_house.txt";

    ASSERT_FALSE(std::filesystem::exists(newDir));

    std::filesystem::path outputFilePath = writer.write(inputFilePath, record);

    ASSERT_TRUE(std::filesystem::exists(newDir));
    ASSERT_TRUE(std::filesystem::exists(outputFilePath));
}


