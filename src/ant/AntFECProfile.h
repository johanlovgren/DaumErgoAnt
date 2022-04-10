//
// Created by Johan Lövgren on 2022-03-30.
//

#ifndef DAUMERGOANT_ANTFECPROFILE_H
#define DAUMERGOANT_ANTFECPROFILE_H

#include <cstdint>
#include <DaumErgo.h>
#include "AntProfile.h"
#include "../../include/antdefines.h"
#include "../../include/dsi_framer_ant.hpp"

class AntFECProfile: public AntProfile {
public:
    explicit AntFECProfile(DaumErgo *ergo, DSIFramerANT *antMessageObject, uint8_t channelNumber);
    ~AntFECProfile();
    void HandleTXEvent(uint8_t *txBuffer) override;

private:
    int transmitPatternCounter;
    int transmitCommonPageCounter;
    uint8_t channelNumber, eventCounter;
    uint16_t accumulatedPower;
    DaumErgo *ergo;
    DSIFramerANT* antMessageObject;

    void TransmitPatternA(uint8_t *buf);
    void DataPage16(uint8_t *buf);
    void DataPage17(uint8_t *buf);
    void DataPage25(uint8_t *buf);
    void DataPage26(uint8_t *buf);
    void DataPage48(uint8_t *buf);
    void DataPage49(uint8_t *buf);
    void DataPage50(uint8_t *buf);
    void DataPage51(uint8_t *buf);
    void DataPage54(uint8_t *buf);
    void DataPage55(uint8_t *buf);
    void DataPage71(uint8_t *buf);
    void DataPage80(uint8_t *buf);
    void DataPage81(uint8_t *buf);


};


#endif //DAUMERGOANT_ANTFECPROFILE_H
