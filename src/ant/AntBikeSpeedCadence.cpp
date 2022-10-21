//
// Created by Johan Lövgren on 2022-10-20.
//

#include <cmath>
#include "AntBikeSpeedCadence.h"
#include "../../include/antdefines.h"

#define SPD_CDC_DEVICE_TYPE ((unsigned char) 121) // Speed & cdc sensor
#define DEVICE_NUM ((unsigned short) 50)
#define TRANS_TYPE ((unsigned char) 1)
#define DEVICE_TYPE ((unsigned short) 121)

#define SPEED_CIRCUMFERENCE 2100 // The circumference of the wheel for speed calculation
#define CHANNEL_PERIOD ((unsigned char) 8086)

AntBikeSpeedCadence::AntBikeSpeedCadence(DaumErgo *ergo) {
    this->ergo = ergo;
    this->crankRevPeriod = this->lastCrankEvent = this->cumulativeCranks = 0;
    this->lastWheelEvent = this->wheelRevPeriod = this->cumulativeWheelRev = 0;
    this->nextCdcTransmit = this->nextSpdTransmit = 0;
    this->deviceType = SPD_CDC_DEVICE_TYPE;
    this->channelPeriod = CHANNEL_PERIOD;
    this->transType = TRANS_TYPE;
    this->channelType = PARAMETER_TX_NOT_RX;
    this->deviceNum = DEVICE_NUM;
}

void AntBikeSpeedCadence::HandleTXEvent(unsigned char *txBuffer) {
    unsigned short cadence = ergo->GetCadence();
    unsigned short speed = ergo->GetSpeed();

    if (!nextCdcTransmit && cadence) {
        cumulativeCranks++;
        // Transforming cadence according to ANT bike power sensor documentation section 6.5.1
        crankRevPeriod = (60 * 1024) / cadence;
        lastCrankEvent += crankRevPeriod;
        nextCdcTransmit = lround(4 * ((double) crankRevPeriod / (double) 1024));
    }
    if (!nextSpdTransmit && speed) {
        cumulativeWheelRev++;
        // Transforming cadence according to ANT bike speed/cadence sensor documentation section 5.5.1
        wheelRevPeriod = 36 * 64 * SPEED_CIRCUMFERENCE / (625 * speed);
        lastWheelEvent += wheelRevPeriod;
        nextSpdTransmit = lround(4 * ((double) wheelRevPeriod / (double) 1024));
    }
    txBuffer[MESSAGE_BUFFER_DATA1_INDEX] = lastCrankEvent; // Last cadence event LSB
    txBuffer[MESSAGE_BUFFER_DATA2_INDEX] = (lastCrankEvent >> 8) & 0xFF; // Last cadence event MSB
    txBuffer[MESSAGE_BUFFER_DATA3_INDEX] = cumulativeCranks; // Cumulative pedal revolutions LSB
    txBuffer[MESSAGE_BUFFER_DATA4_INDEX] = cumulativeCranks >> 8; // Cumulative pedal revolutions MSB
    txBuffer[MESSAGE_BUFFER_DATA5_INDEX] = lastWheelEvent; // Last speed event LSB
    txBuffer[MESSAGE_BUFFER_DATA6_INDEX] = (lastWheelEvent >> 8) & 0xFF; // Last speed event MSB
    txBuffer[MESSAGE_BUFFER_DATA7_INDEX] = cumulativeWheelRev; // Cumulative wheel revolutions LSB
    txBuffer[MESSAGE_BUFFER_DATA8_INDEX] = cumulativeWheelRev >> 8; // Cumulative wheel revolutions MSB

    //pclMessageObject->SendBroadcastData(SPD_CDC_ANTCHANNEL, txBuffer);
    if (speed)
        nextSpdTransmit--;
    if (cadence)
        nextCdcTransmit--;
}
