//
// Created by Johan Lövgren on 2022-10-20.
//
#include "gtest/gtest.h"
#include "../../src/ant/AntBikePower.cpp"
#include "../../src/ant/AntProfile.h"
#include "../../src/daumergo/TestErgo.h"
#include "../../include/antdefines.h"


TEST (ANTBikePower, Constants) {
    AntBikePower antPower = AntBikePower(nullptr);
    ASSERT_EQ(antPower.GetDeviceType(), DEVICE_TYPE);
    ASSERT_EQ(antPower.GetChannelPeriod(), CHANNEL_PERIOD);
    ASSERT_EQ(antPower.GetTransType(), TRANS_TYPE);
    ASSERT_EQ(antPower.GetChannelType(), PARAMETER_TX_NOT_RX);
    ASSERT_EQ(antPower.GetDeviceNum(), DEVICE_NUM);
}



TEST(ANTBikePower, FixedPower) {
    unsigned short fixedPower = 25;
    auto *testErgo = new TestErgo(true);
    testErgo->SetPower(fixedPower);
    AntBikePower bikePower = AntBikePower(testErgo);
    unsigned char txBuffer1[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    unsigned char txBuffer2[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    unsigned short power1, power2, updateEventCount1, updateEventCount2, cumulativePower1, cumulativePower2;

    for (int i = 1; i < 100; i+=2) {
        bikePower.HandleTXEvent((unsigned char*) &txBuffer1);
        bikePower.HandleTXEvent((unsigned char*) &txBuffer2);

        cumulativePower1 = txBuffer1[MESSAGE_BUFFER_DATA5_INDEX] + (txBuffer1[MESSAGE_BUFFER_DATA6_INDEX] << 8);
        power1 = txBuffer1[MESSAGE_BUFFER_DATA7_INDEX] + (txBuffer1[MESSAGE_BUFFER_DATA8_INDEX] << 8);
        updateEventCount1 = txBuffer1[MESSAGE_BUFFER_DATA2_INDEX];

        ASSERT_EQ(cumulativePower1, fixedPower * i);
        ASSERT_EQ(power1, fixedPower);
        ASSERT_EQ(updateEventCount1, i);

        cumulativePower2 = txBuffer2[MESSAGE_BUFFER_DATA5_INDEX] + (txBuffer2[MESSAGE_BUFFER_DATA6_INDEX] << 8);
        power2 = txBuffer2[MESSAGE_BUFFER_DATA7_INDEX] + (txBuffer2[MESSAGE_BUFFER_DATA8_INDEX] << 8);
        updateEventCount2 = txBuffer2[MESSAGE_BUFFER_DATA2_INDEX];

        ASSERT_EQ(cumulativePower2, fixedPower * (i+1));
        ASSERT_EQ(power2, fixedPower);
        ASSERT_EQ(updateEventCount2, i+1);
    }
}
