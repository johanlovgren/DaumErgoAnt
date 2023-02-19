//
// Created by Johan Lövgren on 2022-10-20.
//

#ifndef DAUMERGOANT_ANTBIKESPEEDCADENCE_H
#define DAUMERGOANT_ANTBIKESPEEDCADENCE_H

#include "AntProfile.h"



class AntBikeSpeedCadence: public AntProfile {
public:
    explicit AntBikeSpeedCadence(DaumErgo *ergo);
    /**
      * Handles an ANT+ TX Event
      * @param txBuffer Data that will be sent as broadcast data. Buffer will be filled by this method.
      */
    void HandleTXEvent(uint8_t *txBuffer) override;
    /**
      * Acknowledged events is not implemented for this ANT+ profile
      * @param rxBuf _
      * @param txBuf _
      * @return NOT_SUPPORTED (0xFF)
      */
    uint8_t HandleAckEvent(uint8_t *txBuffer, uint8_t *txBuf) override;

private:
    unsigned short lastCrankEvent, cumulativeCranks, crankRevPeriod;
    unsigned short lastWheelEvent, wheelRevPeriod, cumulativeWheelRev;
    int nextSpdTransmit, nextCdcTransmit;
};


#endif //DAUMERGOANT_ANTBIKESPEEDCADENCE_H
