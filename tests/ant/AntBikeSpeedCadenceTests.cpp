//
// Created by Johan Lövgren on 2022-10-20.
//

#include "gtest/gtest.h"
#include "../../src/ant/AntBikeSpeedCadence.cpp"
#include "../../include/antdefines.h"

TEST (ANTBikeSpeedCadence, Constants) {
    AntBikeSpeedCadence antBikeSpeedCadence = AntBikeSpeedCadence(nullptr);
    ASSERT_EQ(antBikeSpeedCadence.GetDeviceType(), DEVICE_TYPE);
    ASSERT_EQ(antBikeSpeedCadence.GetChannelPeriod(), CHANNEL_PERIOD);
    ASSERT_EQ(antBikeSpeedCadence.GetTransType(), TRANS_TYPE);
    ASSERT_EQ(antBikeSpeedCadence.GetChannelType(), PARAMETER_TX_NOT_RX);
    ASSERT_EQ(antBikeSpeedCadence.GetDeviceNum(), DEVICE_NUM);
}