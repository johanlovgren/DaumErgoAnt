//
// Created by Johan Lövgren on 2022-04-09.
//
#ifndef DAUMERGOANT_ANTPROFILE_H
#define DAUMERGOANT_ANTPROFILE_H

#include "../daumergo/DaumErgo.h"
#include <cstdint>

#define MESSAGE_BUFFER_DATA1_INDEX 0
#define MESSAGE_BUFFER_DATA2_INDEX 1
#define MESSAGE_BUFFER_DATA3_INDEX 2
#define MESSAGE_BUFFER_DATA4_INDEX 3
#define MESSAGE_BUFFER_DATA5_INDEX 4
#define MESSAGE_BUFFER_DATA6_INDEX 5
#define MESSAGE_BUFFER_DATA7_INDEX 6
#define MESSAGE_BUFFER_DATA8_INDEX 7

#define CHANNEL_TYPE_MASTER   (0)
#define CHANNEL_TYPE_SLAVE    (1)
#define CHANNEL_TYPE_INVALID  (2)

#define NOT_SUPPORTED 0xFF


class AntProfile {
public:
    AntProfile();

    /**
      * Handles an ANT+ TX Event and fills buffer for broadcast data
      * @param txBuffer Data that will be sent as broadcast data. Buffer will be filled by this method.
      */
    virtual void HandleTXEvent(unsigned char *txBuffer) = 0;

    /**
      * Handles a Acknowledged Event that was received from slave.
      * @param rxBuf Data received from slave
      * @param txBuf Data that will be returned to slave. Will be filled by method.
      * @return How many times the response is requested to be returned to slave if request was handled successful.
      * If no response shall be sent, 0xFF will be returned.
      */
    virtual uint8_t HandleAckEvent(uint8_t *rxBuf, uint8_t *txBuf) = 0;

    /**
     * Sets the current channel number for the ANT profile
     * @param number the ANT profiles channel number
     */
    void SetChannelNumber(uint8_t number);

    /**
     * Used to retrieve the channel period of the ANT profile
     * @return The ANT profiles channel period
     */
    uint16_t GetChannelPeriod() const;

    /**
     * Used to retrieve the transmission type of the ANT profile
     * @return The ANT profiles transmission type
     */
    uint8_t GetTransType() const;

    /**
     * Used to retrieve the channel type of the ANT profile
     * @return The ANT profiles channel type
     */
    uint8_t GetChannelType() const;

    /**
     * Used to get the device number of the ANT profile
     * @return The ANT profiles device number
     */
    uint8_t GetDeviceNum() const;

    /**
     * Used to get the device type of the ANT profile
     * @return The ANT profiles device type
     */
    uint8_t GetDeviceType() const;

protected:
    DaumErgo *ergo;
    uint8_t transType, channelType, deviceNum, deviceType, channelNumber;
    uint16_t channelPeriod;

};

#endif //DAUMERGOANT_ANTPROFILE_H
