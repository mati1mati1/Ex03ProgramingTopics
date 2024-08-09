#include <gtest/gtest.h>
#include "HouseLocation.hpp"
#include "VacuumHouse.hpp"

TEST(HouseTest, Construction) {
    std::vector<std::string> houseLocations = { 
        { ' ',  '1',  '2'},
        { '3', 'D', '5'},
        { '6',  '7',  '8'}
    };
    uint32_t rows = 3;
    uint32_t cols = 3;
    EXPECT_NO_THROW({
        VacuumHouse house(houseLocations,rows,cols);
    });

    EXPECT_THROW({
        houseLocations = {};
        VacuumHouse house(houseLocations,rows,cols);
    }, std::invalid_argument);
    houseLocations = { 
        { ' ',  '1',  '2'},
        { '3',  '8',  '5'},
        { '6',  '7',  '8'}
    };
    EXPECT_THROW({
        VacuumHouse house(houseLocations,rows,cols); // No charging station
    }, std::invalid_argument);
    
    houseLocations = { 
        { ' ',  '1',  '2'},
        { '3','D',  '5'},
        { '6', 'D',  '8'}
    };
    EXPECT_THROW({
        VacuumHouse house(houseLocations,rows,cols); // Multiple Charging stations
    }, std::invalid_argument);
}

TEST(HouseTest, getAtPosition) {
    uint32_t rows = 3;
    uint32_t cols = 3;
    std::vector<std::string> houseLocations = { 
        { ' ',  '1',  '2'},
        { '3', 'D', '5'},
        { '6',  '7',  '8'}
    };
    VacuumHouse house(houseLocations,rows,cols);
    const HouseLocation& houseLocation2 = house.getCurrentLocation();
    EXPECT_EQ(houseLocation2.getLocationType(),LocationType::CHARGING_STATION);
    EXPECT_EQ(houseLocation2.getDirtLevel(),0);
    
    house.move(Step::South);
    const HouseLocation& houseLocation = house.getCurrentLocation();
    EXPECT_EQ(houseLocation.getLocationType(),LocationType::HOUSE_TILE);
    EXPECT_EQ(houseLocation.getDirtLevel(),7);
    house.move(Step::East);
    const HouseLocation& houseLocation3 = house.getCurrentLocation();
    EXPECT_EQ(houseLocation3.getLocationType(),LocationType::HOUSE_TILE);
    EXPECT_EQ(houseLocation3.getDirtLevel(),8);
}
TEST(HouseTest,move)
{
    uint32_t rows = 3;
    uint32_t cols = 3;
    std::vector<std::string> houseLocations = { 
        { ' ',  '1',  '2'},
        { '3', 'D', '5'},
        { '6',  '7',  '8'}
    };
    VacuumHouse house(houseLocations,rows,cols);
    auto location = house.getCurrentLocation();
    house.move(Step::South);
    auto location2 = house.getCurrentLocation();
    EXPECT_NE(location,location2);
    house.move(Step::East);
    auto location3 = house.getCurrentLocation();
    EXPECT_NE(location2,location3);
    

}
TEST(HouseTest, isMove)
{
    uint32_t rows = 3;
    uint32_t cols = 3;
    std::vector<std::string> houseLocations = { 
        { ' ',  '1',  '2'},
        { '3', 'D', '5'},
        { '6',  '7',  '8'}
    };
    VacuumHouse house(houseLocations,rows,cols);
    EXPECT_TRUE(house.is_move(Step::South));
    EXPECT_TRUE(house.is_move(Step::East));
    EXPECT_TRUE(house.is_move(Step::North));
    EXPECT_TRUE(house.is_move(Step::West));
    house.move(Step::South);
    EXPECT_FALSE(house.is_move(Step::South));
    EXPECT_TRUE(house.is_move(Step::East));
    EXPECT_TRUE(house.is_move(Step::North));
    EXPECT_TRUE(house.is_move(Step::West));
    house.move(Step::East);
    EXPECT_FALSE(house.is_move(Step::South));
    EXPECT_FALSE(house.is_move(Step::East));
    EXPECT_TRUE(house.is_move(Step::North));
    EXPECT_TRUE(house.is_move(Step::West));
}
TEST(HouseTest, outOfBoundsDirtCount)
{
    uint32_t rows = 2;
    uint32_t cols = 3;
    std::vector<std::string> houseLocations = { 
        { ' ',  '1',  '2'},
        { '3', 'D', '5'},
        { '6',  '7',  '8'}
    };
    VacuumHouse house(houseLocations,rows,cols);
    ASSERT_EQ(house.getTotalDirt(),11);
}