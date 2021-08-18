//
// Created by Johan Lövgren on 2021-08-12.
//

#ifndef DAUMERGOANT_ANTSERVICE_H
#define DAUMERGOANT_ANTSERVICE_H


#include "types.h"
#include "dsi_framer_ant.hpp"
#include "dsi_thread.h"
#include "dsi_serial_generic.hpp"
#include "dsi_debug.hpp"
#include "DaumErgoPremium8i.h"

#define CHANNEL_TYPE_MASTER   (0)
#define CHANNEL_TYPE_SLAVE    (1)
#define CHANNEL_TYPE_INVALID  (2)
/**
 * Class used to transmit data from the ANT usb stick.
 * Currently virtualizing a bike power sensor and bike speed/cadence sensor
 */
class AntService {
public:
    /**
     * Default constructor
     * @param ergo the ergo which to transmit data from
     * @param verbose output if true
     */
    AntService(DaumErgo *ergo, bool verbose);
    virtual ~AntService();
    /**
     * Initializes the ANT service
     * @param ucUSBDeviceNumber_ what USB device to use, 0 for first, 1 for second, etc..
     * @return true if successful, otherwise false
     */
    BOOL Init(UCHAR ucUSBDeviceNumber_);
    /**
     * Closes the ANT service, closing the transmissions and the connection to the USB stick
     */
    void Close();



private:
    BOOL InitANT();

    static DSI_THREAD_RETURN RunMessageThread(void *pvParameter_);
    void MessageThread();
    void ProcessMessage(ANT_MESSAGE stMessage);

    void TransmitPower(USHORT power);
    void TransmitSpdCdc(USHORT speed, USHORT cadence);

    DaumErgo *ergo;
    BOOL bVerbose;
    BOOL bBroadcastingSpdCdc;
    BOOL bBroadcastingPower;
    BOOL bMyDone;
    BOOL bDone;
    DSISerialGeneric* pclSerialObject;
    DSIFramerANT* pclMessageObject;
    DSI_THREAD_ID uiDSIThread;
    DSI_CONDITION_VAR condTestDone;
    DSI_MUTEX mutexTestDone;

    BOOL initialised;

    UCHAR aucTransmitBuffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];

    // Power sensor variables
    USHORT cumulativePower;
    UCHAR updateEventCountPower;
    // Cadence sensor variables
    USHORT usLastCrankEvent;
    USHORT usCumCranks;
    USHORT usCrankRevPeriod;
    // Speed sensor variables
    USHORT usLastWheelEvent;
    USHORT usWheelRevPeriod;
    USHORT usCumWheelRev;
    // Variables used to track next transmit
    int nextSpdTransmit;
    int nextCdcTransmit;
};


#endif //DAUMERGOANT_ANTSERVICE_H
