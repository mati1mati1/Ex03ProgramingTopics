#include "gtest/gtest.h"
#include "MeteredVacuumBattery.hpp"

TEST(VaccumTest, ActivateCorrectly) {
  MeteredVacuumBattery vacuumBattery(5,10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.try_activate();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 4);
  vacuumBattery.try_activate(3);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 1);
  vacuumBattery.try_activate(1);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
}

TEST(VaccumTest, ActivateOvercharge) {
  MeteredVacuumBattery vacuumBattery(5,10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.try_activate(6);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
}

TEST(VaccumTest, Charge) {
  MeteredVacuumBattery vacuumBattery(5,10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.charge(2);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 6);
  vacuumBattery.charge(4);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 8);
  vacuumBattery.charge(4);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 10);
}
TEST(VaccumTest, TrikleCharge1) {
  MeteredVacuumBattery vacuumBattery(5,10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.charge();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.charge();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 6);
}
TEST(VaccumTest, TrikleCharge2) {
  MeteredVacuumBattery vacuumBattery(0,4);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(1);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(1);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(1);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(1);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(1);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 1);
}
TEST(VaccumTest, OverCharge) {
  MeteredVacuumBattery vacuumBattery(5,10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.charge(10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 10);
  vacuumBattery.charge(6);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 10);
}
TEST(VaccumTest, TrickleChargeDoubleInnacuracy) {
  MeteredVacuumBattery vacuumBattery(0,3);

  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(6);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(1);
  ASSERT_TRUE(vacuumBattery.try_activate(1));
  vacuumBattery.charge(7);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 1);
  ASSERT_TRUE(vacuumBattery.try_activate(1));
  vacuumBattery.charge(6);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 1);

}
TEST(VaccumTest, OverChargeActivate) {
  MeteredVacuumBattery vacuumBattery(5,10);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.charge(100);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 10);
  vacuumBattery.try_activate();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 9);
  vacuumBattery.charge();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 9);
}
TEST(VaccumTest, Mixed) {
  MeteredVacuumBattery vacuumBattery(5,20);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 5);
  vacuumBattery.charge();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 6);
  vacuumBattery.try_activate(3);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 3);
  vacuumBattery.charge(3);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 6);
  vacuumBattery.try_activate(6);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 0);
  vacuumBattery.charge(10000);
  ASSERT_EQ(vacuumBattery.getBatteryState(), 20);
  vacuumBattery.try_activate();
  ASSERT_EQ(vacuumBattery.getBatteryState(), 19);
}