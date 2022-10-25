//
// Created by Johan Lövgren on 2022-03-30.
//

#ifndef DAUMERGOANT_ANTFECPROFILE_H
#define DAUMERGOANT_ANTFECPROFILE_H

#include <cstdint>
#include <../daumergo/DaumErgo.h>
#include "AntProfile.h"
#include "../../include/antdefines.h"
#include "../../include/dsi_framer_ant.hpp"
#include "../../googletest/googletest/include/gtest/gtest_prod.h"



class AntFECProfile: public AntProfile {
public:
    explicit AntFECProfile(DaumErgo *ergo);
    ~AntFECProfile();
    void HandleTXEvent(uint8_t *txBuffer) override;

private:
    int transmitPatternCounter, transmitPatternBCounter;
    uint8_t eventCounter;
    uint16_t accumulatedPower;

    void TransmitPatternA(uint8_t *buf);
    void TransmitPatternB(uint8_t *buf);

    FRIEND_TEST(ANTFECProfile, TransmitPatternA);
    FRIEND_TEST(ANTFECProfile, TransmitPatternB);
};


#endif //DAUMERGOANT_ANTFECPROFILE_H
