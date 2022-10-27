//
// Created by Johan Lövgren on 2022-01-08.
//

#ifndef DAUMERGOANT_ANTBIKEPOWER_H
#define DAUMERGOANT_ANTBIKEPOWER_H

#include "AntProfile.h"



class AntBikePower: public AntProfile {
public:
    explicit AntBikePower(DaumErgo *ergo);
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
    uint8_t HandleAckEvent(uint8_t *rxBuf, uint8_t *txBuf) override;

private:
    unsigned short cumulativePower;
    unsigned char updateEventCount;

};


#endif //DAUMERGOANT_ANTBIKEPOWER_H
