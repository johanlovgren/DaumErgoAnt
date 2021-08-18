//
// Created by Johan Lövgren on 2021-08-12.
//

#include "DaumErgo.h"

DaumErgo::DaumErgo() {
    usPower = 0;
    usCadence = 0;
    usSpeed = 0;
    bDone = false;
}


unsigned short DaumErgo::GetPower() {
    dataMutex.lock();
    unsigned short power = usPower;
    dataMutex.unlock();
    return power;
}

unsigned short DaumErgo::GetCadence() {
    dataMutex.lock();
    unsigned short cadence = usCadence;
    dataMutex.unlock();
    return cadence;
}

unsigned short DaumErgo::GetSpeed() {
    dataMutex.lock();
    unsigned short speed = usSpeed;
    dataMutex.unlock();
    return speed;
}


