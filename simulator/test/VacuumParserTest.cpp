#include <gtest/gtest.h>
#include "VacuumParser.hpp"

class VacuumParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = VacuumParser();        
    }

    void TearDown() override {

    }
    VacuumParser parser;
};
TEST_F(VacuumParserTest, ParseInvalidHouses)
{
    std::filesystem::path directoryPath = "../../simulator/test/examples/failtests/";
    ASSERT_TRUE(std::filesystem::exists(directoryPath));
    ASSERT_GT(std::filesystem::hard_link_count(directoryPath),0);
    for (const auto & entry : std::filesystem::directory_iterator(directoryPath))
    {
        std::cout << entry.path() << std::endl;
        ASSERT_TRUE(std::filesystem::exists(entry));
        auto vacuum = parser.parse(entry);
        ASSERT_EQ(vacuum,nullptr);
    }
        
}
TEST_F(VacuumParserTest, ParseValidHouses)
{
    std::filesystem::path directoryPath = "../../simulator/test/examples/";
    ASSERT_TRUE(std::filesystem::exists(directoryPath));
    ASSERT_GT(std::filesystem::hard_link_count(directoryPath),0);
    for (const auto & entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.is_directory() || entry.path().filename().string().find("output_") != std::string::npos)
        {
            continue;
        }
        std::cout << entry.path() << std::endl;
        ASSERT_TRUE(std::filesystem::exists(entry));
        auto vacuum = parser.parse(entry);
        ASSERT_NE(vacuum,nullptr);
    }
        
}
TEST_F(VacuumParserTest, CheckParsing)
{
    std::filesystem::path filepath = "../../simulator/test/examples/cleaningTest/house-coridors.house";
    ASSERT_TRUE(std::filesystem::exists(filepath));
    auto vacuum = parser.parse(filepath);
    ASSERT_NE(vacuum,nullptr);
    ASSERT_TRUE(vacuum->getHouse().isWall(Direction::West));
    ASSERT_FALSE(vacuum->getHouse().isWall(Direction::East));
    ASSERT_TRUE(vacuum->getHouse().isWall(Direction::North));
    ASSERT_TRUE(vacuum->getHouse().isWall(Direction::South));

}