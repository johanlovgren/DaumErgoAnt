//
// Created by Johan Lövgren on 2021-08-12.
//

#ifndef DAUMERGOANT_ANTSERVICE_H
#define DAUMERGOANT_ANTSERVICE_H



#include "../../include/types.h"
#include "../../include/dsi_framer_ant.hpp"
#include "../../include/dsi_thread.h"
#include "../../include/dsi_serial_generic.hpp"
#include "../../include/dsi_debug.hpp"
#include "../daumergo/DaumErgoPremium8i.h"
#include "../ant/AntProfile.h"
#include <vector>

using namespace std;

#define MAX_ANT_CHANNELS 8

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
    explicit AntService(bool verbose);
    virtual ~AntService();

    bool AddAntProfile(AntProfile* antProfile);

    /**
     * Initializes the ANT service with the currently added AntProfiles.
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
    void ProcessResponseEvent(ANT_MESSAGE stMessage);
    void ProcessAcknowledgedEvent(ANT_MESSAGE stMessage);
    void ProcessNetworkKeyResponse(ANT_MESSAGE stMessage);
    void ProcessAssignChannelResponse(unsigned char ucChannelNr);
    void ProcessChannelIDResponse(unsigned char ucChannelNr);
    void ProcessChannelRadioFreq(unsigned char ucChannelNr);
    void ProcessMessageEvent(ANT_MESSAGE stMessage, unsigned char ucChannelNr);

    vector<AntProfile*> antProfiles;

    BOOL bVerbose;
    BOOL bDone;
    DSISerialGeneric* pclSerialObject;
    DSIFramerANT* pclMessageObject;
    DSI_THREAD_ID uiDSIThread;
    DSI_CONDITION_VAR condTestDone;
    DSI_MUTEX mutexTestDone;

    BOOL initialised;

    uint8_t nInitialisedProfiles;
    bool currentlyInitialisingProfile;

    unsigned char aucTransmitBuffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];
};


#endif //DAUMERGOANT_ANTSERVICE_H
