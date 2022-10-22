//
// Created by Johan Lövgren on 2022-04-09.
//
#include "AntProfile.h"

AntProfile::AntProfile() {
    channelNumber = -1;
}

void AntProfile::SetChannelNumber(uint8_t number) {
    channelNumber = number;
}

unsigned char AntProfile::GetDeviceType() const {
    return deviceType;
}

unsigned short AntProfile::GetChannelPeriod() const {
    return channelPeriod;
}

unsigned char AntProfile::GetTransType() const {
    return transType;
}

unsigned char AntProfile::GetChannelType() const {
    return channelType;
}

unsigned char AntProfile::GetDeviceNum() const {
    return deviceNum;
}