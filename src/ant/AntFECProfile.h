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
    /**
      * Handles an ANT+ TX Event
      * @param txBuffer Data that will be sent as broadcast data. Buffer will be filled by this method.
      */
    void HandleTXEvent(uint8_t *txBuffer) override;
    /**
      * Handles a Acknowledged Event that was received from slave.
      * @param rxBuf Data received from slave
      * @param txBuf Data that will be returned to slave. Will be filled by method.
      * @return How many times the response is requested to be returned to slave if request was handled successful.
      * If no response shall be sent, 0xFF will be returned.
      */
    uint8_t HandleAckEvent(uint8_t *txBuffer, uint8_t *txBuf) override;
private:
    int transmitPatternCounter, transmitPatternBCounter;
    uint8_t latestReceivedCommandID, latestSlaveSequenceNumber, latestCommandStatus;

    void TransmitPatternA(uint8_t *txBuf);
    void TransmitPatternB(uint8_t *txBuf);

    FRIEND_TEST(ANTFECProfile, TransmitPatternA);
    FRIEND_TEST(ANTFECProfile, TransmitPatternB);
};


#endif //DAUMERGOANT_ANTFECPROFILE_H
