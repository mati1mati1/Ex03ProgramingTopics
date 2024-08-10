#include <gtest/gtest.h>
#include "CleaningRecord.hpp"
#include "CleaningRecordStep.hpp"
#include <filesystem>
#include <fstream>

class OutFileWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        CleaningRecordStep initialStep = CleaningRecordStep(LocationType::CHARGING_STATION, Step::North, 5, 10);
        record = std::make_shared<CleaningRecord>(initialStep,10);
        std::filesystem::create_directory(testOutputDir);
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
    std::filesystem::path outputFilePath = "OutFileWriterTest-working.txt";
    auto fullPath= testOutputDir / outputFilePath;
    std::ofstream outFile(fullPath, std::ios::out );
    writer.write(outFile, record);

    ASSERT_TRUE(std::filesystem::exists(fullPath));
    ASSERT_FALSE(outFile.is_open());
    uint32_t score = 4 + 9 * 300 + 1000;
    std::ifstream outFileRead(fullPath);

    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 4");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 9");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = WORKING");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = FALSE");    
    
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
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
    std::filesystem::path outputFilePath = "OutFileWriterTest-dead.txt";
    auto fullPath= testOutputDir / outputFilePath;

    std::ofstream outFile(fullPath, std::ios::out);
    writer.write(outFile, record);

    ASSERT_TRUE(std::filesystem::exists(fullPath));
    ASSERT_FALSE(outFile.is_open());
    uint32_t score = 10 + 9 * 300 + 2000;
    std::ifstream outFileRead(fullPath);

    std::string line;
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NumSteps = 5");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "DirtLeft = 9");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Status = DEAD");
    
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "InDock = FALSE");    
    
    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Score = " + std::to_string(score));

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "Steps: ");

    std::getline(outFileRead, line);
    EXPECT_EQ(line, "NEsWN");

    outFile.close();
}
TEST_F(OutFileWriterTest, WithoutStep) {
    OutFileWriter writer;
    std::filesystem::path outputFilePath = "OutFileWriterTest-withoutstep.txt";
    auto fullPath= testOutputDir / outputFilePath;
    std::ofstream outFile(fullPath, std::ios::out);

    writer.write(outFile, record);

    ASSERT_TRUE(std::filesystem::exists(fullPath));
    ASSERT_FALSE(outFile.is_open());
    uint32_t score = 0 + 10 * 300 + 0;
    std::ifstream outFileRead(fullPath);
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
    std::filesystem::path outputFilePath = "OutFileWriterTest-finished.txt";
    auto fullPath= testOutputDir / outputFilePath;

    std::ofstream outFile(fullPath, std::ios::out);
    writer.write(outFile, record);
    ASSERT_TRUE(std::filesystem::exists(fullPath));
    ASSERT_FALSE(outFile.is_open());
    std::ifstream outFileRead(fullPath);

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

    outFile.close();
}
/*
TEST_F(OutFileWriterTest, HandleNoRecord) {
    OutFileWriter writer;
    std::filesystem::path outputFilePath = "./HandleNoRecord";
    std::ofstream outFile(outputFilePath, std::ios::out);
    writer.write(outFile, nullptr);
    std::ifstream outFileRead(outputFilePath);
    ASSERT_TRUE(std::filesystem::exists(outputFilePath));
    std::string line;
    std::getline(outFileRead, line);

}

TEST_F(OutFileWriterTest, CreateDirectoryIfNotExists) {
    OutFileWriter writer;
    std::filesystem::path newDir = testOutputDir / "new_dir";
    std::filesystem::path inputFilePath = newDir / "input_house.txt";

    ASSERT_FALSE(std::filesystem::exists(newDir));

    std::filesystem::path outputFilePath = writer.write(inputFilePath, record, "CreateDirectoryIfNotExists");

    ASSERT_TRUE(std::filesystem::exists(newDir));
    ASSERT_TRUE(std::filesystem::exists(outputFilePath));
}


*/