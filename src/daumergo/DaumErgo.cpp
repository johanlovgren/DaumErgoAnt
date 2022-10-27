//
// Created by Johan Lövgren on 2021-08-12.
//

#include "DaumErgo.h"

DaumErgo::DaumErgo() {
    currentPower = 0;
    currentCadence = 0;
    currentSpeed = 0;
    done = false;
}


unsigned short DaumErgo::GetPower() {
    dataMutex.lock();
    unsigned short power = currentPower;
    dataMutex.unlock();
    return power;
}

uint16_t DaumErgo::GetAccumulatedPower() {
    dataMutex.lock();
    uint16_t tmp = accumulatedPower;
    dataMutex.unlock();
    return tmp;
}

uint8_t DaumErgo::GetPowerEventCounter() {
    dataMutex.lock();
    uint8_t tmp = updatePowerEventCount;
    dataMutex.unlock();
    return tmp;
}

uint8_t DaumErgo::GetCadence() {
    dataMutex.lock();
    unsigned short cadence = currentCadence;
    dataMutex.unlock();
    return cadence;
}

uint16_t DaumErgo::GetSpeed() {
    dataMutex.lock();
    unsigned short speed = currentSpeed;
    dataMutex.unlock();
    return speed;
}

uint8_t DaumErgo::GetEquipmentType() const {
    return equipmentType;
}

uint16_t DaumErgo::GetElapsedTime() {
    dataMutex.lock();
    uint16_t tmp = elapsedTime;
    dataMutex.unlock();
    return tmp;
}

uint16_t DaumErgo::GetDistanceTraveled() {
    dataMutex.lock();
    uint16_t tmp = distanceTraveled;
    dataMutex.unlock();
    return tmp;
}

uint8_t DaumErgo::GetHeartRate() const {
    return heartRate;
}

uint8_t DaumErgo::GetHrDistanceSpeedCapabilitiesBits() const {
    return hrDistanceSpeedCapabilitiesBits;
}

uint8_t DaumErgo::GetTrainingModeCapabilities() const {
    return trainingModCapabilities;
}

uint8_t DaumErgo::GetTrainerPowerStatusBitField() {
    dataMutex.lock();
    uint8_t tmp = trainerPowerStatusBitField;
    dataMutex.unlock();
    return tmp;
}

uint8_t DaumErgo::GetFEStateBits() const {
    return feStateBits;
}

uint8_t DaumErgo::GetTargetPowerFlag() {
    dataMutex.lock();
    uint8_t tmp = targetPowerFlag;
    dataMutex.unlock();
    return tmp;
}

uint16_t DaumErgo::GetTargetPower() const {
    return targetPower;
}

uint8_t DaumErgo::GetTargetResistance() const {
    return targetResistance;
}