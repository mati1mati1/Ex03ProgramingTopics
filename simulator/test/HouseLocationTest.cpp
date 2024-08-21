#include <gtest/gtest.h>
#include "HouseLocation.hpp"
TEST(HouseLocationTest, Construction) {
    HouseLocation location_null(' ');
    ASSERT_EQ(LocationType::HOUSE_TILE, location_null.getLocationType());
    ASSERT_EQ(0, location_null.getDirtLevel());
    HouseLocation location_wall(LocationType::WALL);
    ASSERT_EQ(LocationType::WALL, location_wall.getLocationType());
    HouseLocation location_charging(LocationType::CHARGING_STATION);
    ASSERT_EQ(LocationType::CHARGING_STATION, location_charging.getLocationType());
    EXPECT_NO_THROW({
        []() { 
            HouseLocation location_invalid(LocationType::CHARGING_STATION,0); 
            ASSERT_EQ(0, location_invalid.getDirtLevel());
        }();
    });
    EXPECT_NO_THROW({
        []() { 
            HouseLocation location_invalid(LocationType::HOUSE_TILE,0); 
            ASSERT_EQ(0, location_invalid.getDirtLevel());
        }();
    });
    EXPECT_NO_THROW({
        []() { 
            HouseLocation location_invalid(LocationType::WALL,0); 
            ASSERT_EQ(0, location_invalid.getDirtLevel());
        }();
    });
    EXPECT_NO_THROW({
        []() { 
            HouseLocation location_invalid(LocationType::HOUSE_TILE,10); 
            ASSERT_EQ(0, location_invalid.getDirtLevel());
        }();
    });
    HouseLocation location(LocationType::HOUSE_TILE,9); 
    ASSERT_EQ(9, location.getDirtLevel());
}
TEST(HouseLocationTest, SetDirtLevel) {
    HouseLocation location(LocationType::HOUSE_TILE,9); 
    ASSERT_EQ(9, location.getDirtLevel());
    location.setDirtLevel(0);
    ASSERT_EQ(0, location.getDirtLevel());
    location.setDirtLevel(5);
    ASSERT_EQ(5, location.getDirtLevel());
    EXPECT_THROW({
        location.setDirtLevel(10);
    }, std::runtime_error);
    EXPECT_THROW({
        location.setDirtLevel(11);
    }, std::runtime_error);
    EXPECT_THROW({
        location.setDirtLevel(255);
    }, std::runtime_error);
}
TEST(HouseLocationTest, GetDirtLevel) {
    HouseLocation location_null(LocationType::HOUSE_TILE);
    ASSERT_EQ(0, location_null.getDirtLevel());
    HouseLocation location_wall(LocationType::WALL);
    ASSERT_EQ(0, location_wall.getDirtLevel());
    HouseLocation location_charging(LocationType::CHARGING_STATION);
    ASSERT_EQ(0, location_charging.getDirtLevel());
    HouseLocation location(LocationType::HOUSE_TILE,9); 
    ASSERT_EQ(9, location.getDirtLevel());
}