//
// Created by Johan Lövgren on 2022-10-20.
//
#include "gtest/gtest.h"
#include "../../src/ant/AntBikePower.h"
#include "../../include/antdefines.h"

TEST (ANTBikePower, Constants) {
    AntBikePower antPower = AntBikePower(nullptr);
    ASSERT_EQ(antPower.GetDeviceType(), DEVICE_TYPE);
    ASSERT_EQ(antPower.GetChannelPeriod(), CHANNEL_PERIOD);
    ASSERT_EQ(antPower.GetTransType(), TRANS_TYPE);
    ASSERT_EQ(antPower.GetChannelType(), PARAMETER_TX_NOT_RX);
    ASSERT_EQ(antPower.GetDeviceNum(), DEVICE_NUM);
}