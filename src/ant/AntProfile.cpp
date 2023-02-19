//
// Created by Johan Lövgren on 2022-04-09.
//
#include "AntProfile.h"

AntProfile::AntProfile() { }

void AntProfile::SetChannelNumber(uint8_t number) {
    channelNumber = number;
}

uint8_t AntProfile::GetDeviceType() const {
    return deviceType;
}

uint16_t AntProfile::GetChannelPeriod() const {
    return channelPeriod;
}

uint8_t AntProfile::GetTransType() const {
    return transType;
}

uint8_t AntProfile::GetChannelType() const {
    return channelType;
}

uint8_t AntProfile::GetDeviceNum() const {
    return deviceNum;
}