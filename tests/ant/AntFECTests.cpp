//
// Created by Johan Lövgren on 2022-10-24.
//


#include "gtest/gtest.h"
#include "../../src/ant/AntFECProfile.cpp"
#include "../../src/daumergo/TestErgo.h"
#include "../../include/antdefines.h"

TEST (ANTFECProfile, Constants) {
    AntFECProfile antFEC = AntFECProfile(nullptr);
    ASSERT_EQ(antFEC.GetDeviceType(), 17);
    ASSERT_EQ(antFEC.GetChannelPeriod(), 8192);
    ASSERT_EQ(antFEC.GetTransType(), 5);
    ASSERT_EQ(antFEC.GetChannelType(), PARAMETER_TX_NOT_RX);
    ASSERT_EQ(antFEC.GetDeviceNum(), 51);
}

TEST (ANTFECProfile, TransmitPatternB) {
    auto *testErgo = new TestErgo();
    testErgo->SetSpeed(30);
    testErgo->SetCadence(80);
    testErgo->SetPower(200);
    AntFECProfile antFEC = AntFECProfile(testErgo);
    int transmitCounter = 0;
    int transmitCounterB = 0;
    uint8_t buf[8];
    // Transmit pattern limits taken from ANT FEC documentation
    for (int i = 0; i < 10000; ++i) {
        antFEC.TransmitPatternB(buf);
        transmitCounter++;
        if (transmitCounter >= 0 && transmitCounter < 64) {
            transmitCounterB++;
            if (transmitCounterB >= 0 && transmitCounterB < 2) {
                ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
            } else if (transmitCounterB >= 2 && transmitCounterB < 4) {
                ASSERT_EQ(buf[DP_NUMBER_INDEX], 25);
            } else {
                ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
                transmitCounterB = 0;
            }
        } else if (transmitCounter >= 64 && transmitCounter < 66) {
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 80);
        } else if (transmitCounter >= 66 && transmitCounter < 130) {
            transmitCounterB++;
            if (transmitCounterB >= 0 && transmitCounterB < 2) {
                ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
            } else if (transmitCounterB >= 2 && transmitCounterB < 4) {
                ASSERT_EQ(buf[DP_NUMBER_INDEX], 25);
            } else {
                ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
                transmitCounterB = 0;
            }
        } else if (transmitCounter >= 130 && transmitCounter < 132) {
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 81);
        } else {
            transmitCounter = 0;
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
        }
    }
    testErgo->Close();
}

TEST (ANTFECProfile, TransmitPatternA) {
    auto *testErgo = new TestErgo();
    testErgo->SetSpeed(30);
    testErgo->SetCadence(80);
    testErgo->SetPower(200);
    AntFECProfile antFEC = AntFECProfile(testErgo);
    int transmitCounter = 0;
    uint8_t buf[8];
    // Transmit pattern limits taken from ANT FEC documentation
    for (int i = 0; i < 10000; ++i) {
        antFEC.TransmitPatternA(buf);
        transmitCounter++;
        if (transmitCounter >= 0 && transmitCounter < 64) {
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
        } else if (transmitCounter >= 64 && transmitCounter < 66) {
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 80);
        } else if (transmitCounter >= 66 && transmitCounter < 130) {
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
        } else if (transmitCounter >= 130 && transmitCounter < 132) {
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 81);
        } else {
            transmitCounter = 0;
            ASSERT_EQ(buf[DP_NUMBER_INDEX], 16);
        }
    }
    testErgo->Close();
}

