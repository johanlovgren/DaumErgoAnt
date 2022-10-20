//
// Created by Johan Lövgren on 2022-04-09.
//
#ifndef DAUMERGOANT_ANTPROFILE_H
#define DAUMERGOANT_ANTPROFILE_H

#include "../daumergo/DaumErgo.h"
#include <cstdint>

#define MESSAGE_BUFFER_DATA1_INDEX 0
#define MESSAGE_BUFFER_DATA2_INDEX 1
#define MESSAGE_BUFFER_DATA3_INDEX 2
#define MESSAGE_BUFFER_DATA4_INDEX 3
#define MESSAGE_BUFFER_DATA5_INDEX 4
#define MESSAGE_BUFFER_DATA6_INDEX 5
#define MESSAGE_BUFFER_DATA7_INDEX 6
#define MESSAGE_BUFFER_DATA8_INDEX 7


class AntProfile {
public:
    AntProfile();
    virtual void HandleTXEvent(uint8_t *txBuffer) = 0;

    void SetChannelNumber(uint8_t number);
    unsigned char GetChannelPeriod() const;
    unsigned char GetTransType() const;
    unsigned char GetChannelType() const;
    unsigned char GetDeviceNum() const;
    unsigned short GetDeviceType() const;

protected:
    DaumErgo *ergo;
    uint8_t channelNumber;
    unsigned short deviceType;
    unsigned char channelPeriod, transType, channelType, deviceNum;

};
#endif //DAUMERGOANT_ANTPROFILE_H
