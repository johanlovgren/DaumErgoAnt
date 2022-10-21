//
// Created by Johan Lövgren on 2022-10-20.
//

#ifndef DAUMERGOANT_ANTBIKESPEEDCADENCE_H
#define DAUMERGOANT_ANTBIKESPEEDCADENCE_H

#include "AntProfile.h"



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
