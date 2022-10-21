//
// Created by Johan Lövgren on 2022-01-08.
//

#include "AntBikePower.h"
#include "../../include/antdefines.h"

#define CHANNEL_PERIOD ((unsigned char) 8182)
#define TRANS_TYPE ((unsigned char) 1)
#define DEVICE_NUM ((unsigned short) 49)
#define DEVICE_TYPE ((unsigned char) 11)


AntBikePower::AntBikePower(DaumErgo *ergo) {
    this->ergo = ergo;
    this->channelPeriod = CHANNEL_PERIOD;
    this->transType = TRANS_TYPE;
    this->channelType = PARAMETER_TX_NOT_RX;
    this->deviceType = DEVICE_TYPE;
    this->deviceNum = DEVICE_NUM;
    this->updateEventCount = 0;
}

void AntBikePower::HandleTXEvent(unsigned char *txBuffer) {
    unsigned short power = ergo->GetPower();
    cumulativePower += power;

    txBuffer[0] = 0x10; // Standard Power-Only message
    txBuffer[1] = ++updateEventCount; // Update event count
    txBuffer[2] = 0xFF; // Pedal Power off
    txBuffer[3] = 0xFF; // Cadence set invalid
    txBuffer[4] = cumulativePower; // Accumulated power LSB
    txBuffer[5] = (cumulativePower >> 8) & 0xFF; // Accumulated power MSB
    txBuffer[6] = power; // Power LSB
    txBuffer[7] = power >> 8; // Power MSB

    //pclMessageObject->SendBroadcastData(POWER_ANTCHANNEL, txBuffer);
}