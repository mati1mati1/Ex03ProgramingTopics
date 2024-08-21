#include <gtest/gtest.h>
#include "CleaningRecord.hpp"

TEST(CleaningRecordTest, Construction) {
    CleaningRecordStep step = CleaningRecordStep(LocationType::CHARGING_STATION, Step::North, 1, 2);
    CleaningRecord record(step, 10);
    ASSERT_EQ(record.size(), 0);
    ASSERT_NO_THROW(record.last());
    ASSERT_NO_THROW(record[0]);
}

TEST(CleaningRecordTest, Add) {
    CleaningRecordStep step = CleaningRecordStep(LocationType::CHARGING_STATION, Step::North, 1, 2);
    CleaningRecord record(step, 10);
    ASSERT_EQ(record.size(), 0);
    CleaningRecordStep step2 = CleaningRecordStep(LocationType::CHARGING_STATION, Step::South, 10, 20);
    record.add(step2);
    ASSERT_EQ(record.size(), 1);

    ASSERT_EQ(*record[0], step2);
    ASSERT_EQ(*record.last(), step2);
    ASSERT_EQ(record[0]->getLocationType(), LocationType::CHARGING_STATION);
    ASSERT_EQ(record[0]->getStep(), Step::South);
    ASSERT_EQ(record[0]->getBatteryLevel(), 10);
    ASSERT_EQ(record[0]->getDirtLevel(), 20);
}

TEST(CleaningRecordTest, InitialValue) {
    CleaningRecordStep step = CleaningRecordStep(LocationType::CHARGING_STATION, Step::North, 1, 2);
    CleaningRecord record(step, 10);
    ASSERT_EQ(record.size(), 0);
    ASSERT_EQ(*record.getInitialStep(), step);
    ASSERT_EQ(record.size(), 0);
    ASSERT_EQ(record[0], record.getInitialStep());
    ASSERT_EQ(record.last(), record.getInitialStep());
    CleaningRecordStep otherStep = CleaningRecordStep(LocationType::CHARGING_STATION, Step::South, 10, 20);
    record.add(otherStep);
    ASSERT_EQ(record.size(), 1);

    ASSERT_EQ(record.getInitialStep(), nullptr);
    ASSERT_EQ(*record[0], otherStep);
    ASSERT_EQ(*record.last(), otherStep);
}