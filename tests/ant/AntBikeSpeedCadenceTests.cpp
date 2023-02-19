//
// Created by Johan Lövgren on 2022-10-20.
//

#include "gtest/gtest.h"
#include "../../src/ant/AntBikeSpeedCadence.cpp"
#include "../../src/daumergo/TestErgo.h"
#include "../../include/antdefines.h"

TEST (ANTBikeSpeedCadence, Constants) {
    AntBikeSpeedCadence antBikeSpeedCadence = AntBikeSpeedCadence(nullptr);
    ASSERT_EQ(antBikeSpeedCadence.GetDeviceType(), 121);
    ASSERT_EQ(antBikeSpeedCadence.GetChannelPeriod(), 8086);
    ASSERT_EQ(antBikeSpeedCadence.GetTransType(), 1);
    ASSERT_EQ(antBikeSpeedCadence.GetChannelType(), PARAMETER_TX_NOT_RX);
    ASSERT_EQ(antBikeSpeedCadence.GetDeviceNum(), 50);
}

TEST(ANTBikeSpeedCadence, FixedSpeedCadence) {
    unsigned short fixedSpeed = 30;
    unsigned short fixedCadence = 80;
    auto *testErgo = new TestErgo();
    testErgo->SetSpeed(fixedSpeed);
    testErgo->SetCadence(fixedCadence);
    AntBikeSpeedCadence bikeSpeedCadence = AntBikeSpeedCadence(testErgo);

    unsigned char txBuffer1[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    unsigned char txBuffer2[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    unsigned short cadenceEventTime1, cadenceEventTime2, cumulativeCadenceRev1, cumulativeCadenceRev2;
    unsigned short speedEventTime1, speedEventTime2, cumulativeSpeedRev1, cumulativeSpeedRev2;
    unsigned short currentCadence, currentSpeed;

    for (int i = 1; i < 10000; i+=1) {
        bikeSpeedCadence.HandleTXEvent((unsigned char*) &txBuffer1);
        bikeSpeedCadence.HandleTXEvent((unsigned char*) &txBuffer2);
        if (txBuffer1[MESSAGE_BUFFER_DATA1_INDEX] == txBuffer2[MESSAGE_BUFFER_DATA1_INDEX])
            continue;

        cadenceEventTime1 = txBuffer1[MESSAGE_BUFFER_DATA1_INDEX] + (txBuffer1[MESSAGE_BUFFER_DATA2_INDEX] << 8);
        cadenceEventTime2 = txBuffer2[MESSAGE_BUFFER_DATA1_INDEX] + (txBuffer2[MESSAGE_BUFFER_DATA2_INDEX] << 8);
        cumulativeCadenceRev1 = txBuffer1[MESSAGE_BUFFER_DATA3_INDEX] + (txBuffer1[MESSAGE_BUFFER_DATA4_INDEX] << 8);
        cumulativeCadenceRev2 = txBuffer2[MESSAGE_BUFFER_DATA3_INDEX] + (txBuffer2[MESSAGE_BUFFER_DATA4_INDEX] << 8);

        speedEventTime1 = txBuffer1[MESSAGE_BUFFER_DATA5_INDEX] + (txBuffer1[MESSAGE_BUFFER_DATA6_INDEX] << 8);
        speedEventTime2 = txBuffer2[MESSAGE_BUFFER_DATA5_INDEX] + (txBuffer2[MESSAGE_BUFFER_DATA6_INDEX] << 8);
        cumulativeSpeedRev1 = txBuffer1[MESSAGE_BUFFER_DATA7_INDEX] + (txBuffer1[MESSAGE_BUFFER_DATA8_INDEX] << 8);
        cumulativeSpeedRev2 = txBuffer2[MESSAGE_BUFFER_DATA7_INDEX] + (txBuffer2[MESSAGE_BUFFER_DATA8_INDEX] << 8);

        currentCadence = (60 * 1024 * (cumulativeCadenceRev2 - cumulativeCadenceRev1)) /
                (cadenceEventTime2 - cadenceEventTime1);
        currentSpeed = SPEED_CIRCUMFERENCE * 1024 * (cumulativeSpeedRev2 - cumulativeSpeedRev1) /
                (speedEventTime2 - speedEventTime1);

        ASSERT_EQ(fixedCadence, currentCadence);
        ASSERT_EQ(fixedSpeed, currentSpeed);
    }
    testErgo->Close();
}