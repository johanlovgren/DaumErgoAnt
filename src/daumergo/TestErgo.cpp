//
// Created by Johan Lövgren on 2022-10-20.
//

#include <vector>
#include <cmath>
#include "TestErgo.h"

#define MAXIMUM_WATT 800
#define MINIMUM_WATT 25

TestErgo::TestErgo() {
    this->hrDistanceSpeedCapabilitiesBits = 0;
    this->feStateBits = 3;
    this->equipmentType = 25;
    this->distanceTraveled = 0;
    this->heartRate = 0;
    this->elapsedTime = 0;
    this->trainerPowerStatusBitField = 0;
    this->feStateBits = 3; // InUse
    this->targetPowerFlag = 0; // Operating at target power
    // Training mode capabilities: Bit 0 Basic resistance, bit 1 target power, bit 2 sim mode, bit 3-7 set 0
    trainingModCapabilities = 0 + (1 << 1);
}


bool TestErgo::Init(const char *string) {
    return true;
}

void TestErgo::Close() {
    done = true;
}

bool TestErgo::SetPower(uint16_t power) {
    power = (uint16_t) round(power / 5) * 5;
    this->currentPower = power;
    return true;
}

bool  TestErgo::SetResistance(uint8_t resistance) {
    targetResistance = resistance;
    uint16_t power = MAXIMUM_WATT * ((double) resistance / 100);
    SetPower(power);
    return false;
}

void TestErgo::SetCadence(unsigned short cadence) {
    this->currentCadence = cadence;
}

void TestErgo::SetSpeed(unsigned short speed) {
    this->currentSpeed = speed;
}

void TestErgo::SetEquipmentType(uint8_t equipmentType) {
    this->equipmentType = equipmentType;
}

void TestErgo::DataUpdater() {
    while (!done) {
        this->accumulatedPower += currentPower;
        this->updatePowerEventCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool TestErgo::RunDataUpdater() {
    done = false;
    std::thread(&TestErgo::DataUpdater, this).detach();
    return true;
}

bool TestErgo::RunWorkout(std::vector<std::tuple<int, int>> workout) {
    return false;
}

uint8_t TestErgo::GetCycleLength() {
    return 0;
}

uint8_t TestErgo::GetResistanceLevel() {
    return 0;
}

uint16_t TestErgo::GetIncline() {
    return 0;
}
