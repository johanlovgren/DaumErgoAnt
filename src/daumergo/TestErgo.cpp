//
// Created by Johan Lövgren on 2022-10-20.
//

#include <vector>
#include "TestErgo.h"

TestErgo::TestErgo() {
    this->capabilitiesFeState = 0 + (3 << 4);
    this->equipmentType = 25;
    this->distanceTraveled = 0;
    this->heartRate = 0;
    this->elapsedTime = 0;
    this->trainerPowerStatusBitField = 0;
    this->feStateBits = 3; // InUse
    this->targetPowerFlag = 0; // Operating at target power
}


bool TestErgo::Init(const char *string) {
    return true;
}

void TestErgo::Close() {
    done = true;
}

void TestErgo::SetPower(unsigned short power) {
    this->currentPower = power;
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
