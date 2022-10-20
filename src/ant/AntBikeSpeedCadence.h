//
// Created by Johan Lövgren on 2022-10-20.
//

#ifndef DAUMERGOANT_ANTBIKESPEEDCADENCE_H
#define DAUMERGOANT_ANTBIKESPEEDCADENCE_H

#include "AntProfile.h"

#define SPD_CDC_DEVICE_TYPE ((unsigned char) 121) // Speed & cdc sensor
#define DEVICE_NUM ((unsigned short) 50)
#define TRANS_TYPE ((unsigned char) 1)
#define DEVICE_TYPE ((unsigned short) 121)

#define SPEED_CIRCUMFERENCE 2100 // The circumference of the wheel for speed calculation
#define CHANNEL_PERIOD ((unsigned char) 8086)

class AntBikeSpeedCadence: public AntProfile {
public:
    explicit AntBikeSpeedCadence(DaumErgo *ergo);
    void HandleTXEvent(uint8_t *txBuffer) override;

private:
    unsigned short lastCrankEvent, cumulativeCranks, crankRevPeriod;
    unsigned short lastWheelEvent, wheelRevPeriod, cumulativeWheelRev;
    int nextSpdTransmit, nextCdcTransmit;
};


#endif //DAUMERGOANT_ANTBIKESPEEDCADENCE_H
