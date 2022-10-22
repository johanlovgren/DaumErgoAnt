//
// Created by Johan Lövgren on 2022-10-20.
//

#include <vector>
#include "TestErgo.h"

TestErgo::TestErgo() { }


bool TestErgo::Init(const char *string) {
    return true;
}

void TestErgo::Close() { }

void TestErgo::SetPower(unsigned short power) {
    this->currentPower = power;
}

void TestErgo::SetCadence(unsigned short cadence) {
    this->currentCadence = cadence;
}

void TestErgo::SetSpeed(unsigned short speed) {
    this->currentSpeed = speed;
}

bool TestErgo::RunDataUpdater() {
    return false;
}

bool TestErgo::RunWorkout(std::vector<std::tuple<int, int>> workout) {
    return false;
}

uint8_t TestErgo::GetEquipmentType() {
    return 0;
}

uint16_t TestErgo::GetElapsedTime() {
    return 0;
}

uint16_t TestErgo::GetDistanceTraveled() {
    return 0;
}

uint8_t TestErgo::GetHeartRate() {
    return 0;
}

uint8_t TestErgo::GetCapabilitiesState() {
    return 0;
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

uint8_t TestErgo::GetFEStateBits() {
    return 0;
}

uint8_t TestErgo::GetTargetPowerFlag() {
    return 0;
}

uint8_t TestErgo::GetTrainerStatusBitField() {
    return 0;
}
