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

unsigned short DaumErgo::GetCadence() {
    dataMutex.lock();
    unsigned short cadence = currentCadence;
    dataMutex.unlock();
    return cadence;
}

unsigned short DaumErgo::GetSpeed() {
    dataMutex.lock();
    unsigned short speed = currentSpeed;
    dataMutex.unlock();
    return speed;
}


