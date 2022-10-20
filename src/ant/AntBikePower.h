//
// Created by Johan Lövgren on 2022-01-08.
//

#ifndef DAUMERGOANT_ANTBIKEPOWER_H
#define DAUMERGOANT_ANTBIKEPOWER_H

#include "AntProfile.h"

#define CHANNEL_PERIOD ((unsigned char) 8182)
#define TRANS_TYPE ((unsigned char) 1)
#define DEVICE_NUM ((unsigned short) 49)
#define DEVICE_TYPE ((unsigned short) 11)

class AntBikePower: public AntProfile {
public:
    explicit AntBikePower(DaumErgo *ergo);
    void HandleTXEvent(uint8_t *txBuffer) override;

private:
    unsigned short cumulativePower;
    unsigned char updateEventCount;

};


#endif //DAUMERGOANT_ANTBIKEPOWER_H
