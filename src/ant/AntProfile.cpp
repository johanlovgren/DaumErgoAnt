//
// Created by Johan Lövgren on 2022-04-09.
//
#include "AntProfile.h"

AntProfile::AntProfile() {
    ChannelNumber = -1;
}

void AntProfile::SetChannelNumber(uint8_t number) {
    ChannelNumber = number;
}