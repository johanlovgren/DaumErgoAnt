//
// Created by Johan Lövgren on 2022-01-08.
//

#include "AntBikePower.h"
#include "../../include/antdefines.h"

#define CHANNEL_PERIOD ((unsigned short) 8182)
#define TRANS_TYPE ((unsigned char) 5)
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
    this->cumulativePower = 0;
}

void AntBikePower::HandleTXEvent(unsigned char *txBuffer) {
    unsigned short power = ergo->GetPower();
    cumulativePower += power;

    txBuffer[MESSAGE_BUFFER_DATA1_INDEX] = 0x10; // Standard Power-Only message
    txBuffer[MESSAGE_BUFFER_DATA2_INDEX] = ++updateEventCount; // Update event count
    txBuffer[MESSAGE_BUFFER_DATA3_INDEX] = 0xFF; // Pedal Power off
    txBuffer[MESSAGE_BUFFER_DATA4_INDEX] = 0xFF; // Cadence set invalid
    txBuffer[MESSAGE_BUFFER_DATA5_INDEX] = cumulativePower; // Accumulated power LSB
    txBuffer[MESSAGE_BUFFER_DATA6_INDEX] = (cumulativePower >> 8) & 0xFF; // Accumulated power MSB
    txBuffer[MESSAGE_BUFFER_DATA7_INDEX] = power; // Power LSB
    txBuffer[MESSAGE_BUFFER_DATA8_INDEX] = power >> 8; // Power MSB

    //pclMessageObject->SendBroadcastData(POWER_ANTCHANNEL, txBuffer);
}