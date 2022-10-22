//
// Created by Johan Lövgren on 2022-10-20.
//

#include "gtest/gtest.h"
#include "../../src/ant/AntBikeSpeedCadence.cpp"
#include "../../include/antdefines.h"

TEST (ANTBikeSpeedCadence, Constants) {
    AntBikeSpeedCadence antBikeSpeedCadence = AntBikeSpeedCadence(nullptr);
    ASSERT_EQ(antBikeSpeedCadence.GetDeviceType(), 121);
    ASSERT_EQ(antBikeSpeedCadence.GetChannelPeriod(), 8086);
    ASSERT_EQ(antBikeSpeedCadence.GetTransType(), 1);
    ASSERT_EQ(antBikeSpeedCadence.GetChannelType(), PARAMETER_TX_NOT_RX);
    ASSERT_EQ(antBikeSpeedCadence.GetDeviceNum(), 50);
}