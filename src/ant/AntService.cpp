//
// Created by Johan Lövgren on 2021-08-12.
// Based on ANT example code from thisisant.com
//

#include "AntService.h"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
#include <cmath>

// Includes network_key as a config
#include "../../../network_key.cfg"




#define ENABLE_EXTENDED_MESSAGES

#define USER_BAUDRATE         (57600)  // For AT3/AP2, use 57600
#define USER_RADIOFREQ        (57) // RF Frequency + 2400

#define POWER_ANTCHANNEL   (1) // Power sensor channel
#define SPD_CDC_ANTCHANNEL (0) // Speed sensor channel
#define POWER_DEVICENUM    (49) // Power sensor number
#define SPD_CDC_DEVICENUM  (50) // Speed sensor
#define POWER_DEVICETYPE   (11) // Power sensor
#define SPD_CDC_DEVICETYPE (121)// Speed & cdc sensor
#define USER_TRANSTYPE     (1)
#define POWER_PERIOD       (8182)
#define SPD_CDC_PERIOD     (8086)
#define SPEED_CIRCUMFERENCE    (2100) // The circumference of the wheel for speed calculation

#define USER_NETWORK_NUM      (0)      // The network key is assigned to this network number

#define MESSAGE_TIMEOUT       (1000)

// Indexes into message recieved from ANT
#define MESSAGE_BUFFER_DATA1_INDEX ((UCHAR) 0)
#define MESSAGE_BUFFER_DATA2_INDEX ((UCHAR) 1)
#define MESSAGE_BUFFER_DATA3_INDEX ((UCHAR) 2)
#define MESSAGE_BUFFER_DATA4_INDEX ((UCHAR) 3)
#define MESSAGE_BUFFER_DATA5_INDEX ((UCHAR) 4)
#define MESSAGE_BUFFER_DATA6_INDEX ((UCHAR) 5)
#define MESSAGE_BUFFER_DATA7_INDEX ((UCHAR) 6)
#define MESSAGE_BUFFER_DATA8_INDEX ((UCHAR) 7)
#define MESSAGE_BUFFER_DATA9_INDEX ((UCHAR) 8)
#define MESSAGE_BUFFER_DATA10_INDEX ((UCHAR) 9)
#define MESSAGE_BUFFER_DATA11_INDEX ((UCHAR) 10)
#define MESSAGE_BUFFER_DATA12_INDEX ((UCHAR) 11)
#define MESSAGE_BUFFER_DATA13_INDEX ((UCHAR) 12)
#define MESSAGE_BUFFER_DATA14_INDEX ((UCHAR) 13)

// ----------------------- Public -------------------------------

AntService::AntService(DaumErgo *ergo, bool verbose)
{
    pclSerialObject = (DSISerialGeneric*)nullptr;
    pclMessageObject = (DSIFramerANT*)nullptr;
    uiDSIThread = (DSI_THREAD_ID)nullptr;
    bMyDone = FALSE;
    bDone = FALSE;
    bBroadcastingSpdCdc = FALSE;
    bBroadcastingPower = FALSE;

    initialised = FALSE;


    this->ergo = ergo;
    bVerbose = verbose;
    cumulativePower = 0;
    updateEventCountPower = 0;

    usCumWheelRev = 0;
    usLastWheelEvent = 0;
    usCumCranks = 0;
    usLastCrankEvent = 0;

    usWheelRevPeriod = 0;
    usCrankRevPeriod = 0;

    nextSpdTransmit = 0;
    nextCdcTransmit = 0;

    memset(aucTransmitBuffer,0,ANT_STANDARD_DATA_PAYLOAD_SIZE);
}

AntService::~AntService()
{
    delete pclMessageObject;
    delete pclSerialObject;
}

BOOL AntService::Init(UCHAR ucUSBDeviceNumber_)
{

    BOOL bStatus;

    // Initialize condition var and mutex
    UCHAR ucCondInit = DSIThread_CondInit(&condTestDone);
    assert(ucCondInit == DSI_THREAD_ENONE);

    UCHAR ucMutexInit = DSIThread_MutexInit(&mutexTestDone);
    assert(ucMutexInit == DSI_THREAD_ENONE);

#if defined(DEBUG_FILE)
    // Enable logging
    DSIDebug::Init();
    DSIDebug::SetDebug(TRUE);
#endif

    // Create Serial object.
    pclSerialObject = new DSISerialGeneric();
    assert(pclSerialObject);

    // Initialize Serial object.
    // The device number depends on how many USB sticks have been
    // plugged into the PC. The first USB stick plugged will be 0
    // the next 1 and so on.
    //
    // The Baud Rate depends on the ANT solution being used. AP1
    // is 50000, all others are 57600
    bStatus = pclSerialObject->Init(USER_BAUDRATE, ucUSBDeviceNumber_);
    assert(bStatus);

    // Create Framer object.
    pclMessageObject = new DSIFramerANT(pclSerialObject);
    assert(pclMessageObject);

    // Initialize Framer object.
    bStatus = pclMessageObject->Init();
    assert(bStatus);

    // Let Serial know about Framer.
    pclSerialObject->SetCallback(pclMessageObject);

    // Open Serial.
    bStatus = pclSerialObject->Open();

    // If the Open function failed, most likely the device
    // we are trying to access does not exist, or it is connected
    // to another program
    if(!bStatus)
    {
        std::cerr << "Failed to connect to ANT device at USB port " << ucUSBDeviceNumber_ << std::endl;
        return FALSE;
    }

    // Create message thread.
    uiDSIThread = DSIThread_CreateThread(&AntService::RunMessageThread, this);
    assert(uiDSIThread);

    // Start ANT channel setup
    if (!InitANT()) {
        std::cerr << "Error while initializing ANT Service" << std::endl;
        return false;
    }
    initialised = TRUE;
    if (bVerbose)
        std::cout << "ANT Service initialized" << std::endl;

    return TRUE;
}

void AntService::Close()
{
    if (!initialised)
        return;
    bBroadcastingSpdCdc = FALSE;
    bBroadcastingPower = FALSE;
    pclMessageObject->CloseChannel(SPD_CDC_ANTCHANNEL, MESSAGE_TIMEOUT);
    pclMessageObject->CloseChannel(POWER_ANTCHANNEL, MESSAGE_TIMEOUT);

    DSIThread_Sleep(0);

    //Wait for test to be done
    DSIThread_MutexLock(&mutexTestDone);
    bDone = TRUE;

    UCHAR ucWaitResult = DSIThread_CondTimedWait(&condTestDone, &mutexTestDone, DSI_THREAD_INFINITE);
    assert(ucWaitResult == DSI_THREAD_ENONE);

    DSIThread_MutexUnlock(&mutexTestDone);

    //Destroy mutex and condition var
    DSIThread_MutexDestroy(&mutexTestDone);
    DSIThread_CondDestroy(&condTestDone);

    //Close all stuff
    if(pclSerialObject)
        pclSerialObject->Close();

#if defined(DEBUG_FILE)
    DSIDebug::Close();
#endif

}

// ----------------------- Private ------------------------------

/**
 * Prints a transmission buffer
 * @param aucTransmitBuffer buffer to print
 */
void printTransmissionBuffer(UCHAR *aucTransmitBuffer) {
    printf("Tx: [%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x]\n",
           aucTransmitBuffer[MESSAGE_BUFFER_DATA1_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA2_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA3_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA4_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA5_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA6_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA7_INDEX],
           aucTransmitBuffer[MESSAGE_BUFFER_DATA8_INDEX]);
}


/**
 * Initializes the ANT service, resetting system and setting network key
 * @return
 */
BOOL AntService::InitANT()
{
    BOOL bStatus;

    // Reset system
    if (bVerbose)
        std::cout << "Resetting module" << std::endl;
    if (!pclMessageObject->ResetSystem())
        return FALSE;
    DSIThread_Sleep(1000);

    if (bVerbose)
        std::cout << "Setting network key.. " << std::endl;
    UCHAR ucNetKey[] = NETWORK_KEY;

    bStatus = pclMessageObject->SetNetworkKey(USER_NETWORK_NUM, ucNetKey, MESSAGE_TIMEOUT);
    if (!bStatus) {
        std::cerr << "Error initializing network key" << std::endl;
        return FALSE;
    }

    return bStatus;
}

/**
 * Callback function used to create the DSI thread
 * @param pvParameter_
 * @return
 */
DSI_THREAD_RETURN AntService::RunMessageThread(void *pvParameter_)
{
    ((AntService*) pvParameter_)->MessageThread();
    return nullptr;
}

/**
 * Entry point for message thread. Checks for received messages and processes these
 */
void AntService::MessageThread()
{
    ANT_MESSAGE stMessage;
    USHORT usSize;
    bDone = FALSE;

    while(!bDone)
    {
        if(pclMessageObject->WaitForMessage(1000))
        {
            usSize = pclMessageObject->GetMessage(&stMessage);

            if(bDone)
                break;

            if(usSize == DSI_FRAMER_ERROR)
            {
                // Get the message to clear the error
                pclMessageObject->GetMessage(&stMessage, MESG_MAX_SIZE_VALUE);
                continue;
            }

            if(usSize != DSI_FRAMER_ERROR && usSize != DSI_FRAMER_TIMEDOUT && usSize != 0)
            {
                ProcessMessage(stMessage);
            }
        }
    }

    DSIThread_MutexLock(&mutexTestDone);
    UCHAR ucCondResult = DSIThread_CondSignal(&condTestDone);
    assert(ucCondResult == DSI_THREAD_ENONE);
    DSIThread_MutexUnlock(&mutexTestDone);

}



/**
 * Process the received messages, including event messages and thus ensures that channels are initialized properly
 * @param stMessage message received
 */
void AntService::ProcessMessage(ANT_MESSAGE stMessage)
{
    switch(stMessage.ucMessageID)
    {
        //RESPONSE MESG
        case MESG_RESPONSE_EVENT_ID:
        {
            UCHAR ucChannelNr = pclMessageObject->GetChannelNumber(&stMessage);
            //RESPONSE TYPE
            switch(stMessage.aucData[1])
            {
                case MESG_NETWORK_KEY_ID:
                {
                    if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error configuring network key: Code " << stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose) {
                        std::cout << "Network key set" << std::endl;
                        std::cout << "Assigning channel: " << POWER_ANTCHANNEL << std::endl;
                    }
                    pclMessageObject->AssignChannel(POWER_ANTCHANNEL,
                                                    PARAMETER_TX_NOT_RX,
                                                    0,
                                                    MESSAGE_TIMEOUT);
                    break;
                }

                case MESG_ASSIGN_CHANNEL_ID:
                {
                    if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error assigning channel: Code " << stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose) {
                        std::cout << "Channel assigned " << ucChannelNr << std::endl;
                        std::cout << "Setting channel ID" << std::endl;
                    }
                    if (ucChannelNr == POWER_ANTCHANNEL) {
                        pclMessageObject->SetChannelID(POWER_ANTCHANNEL,
                                                       POWER_DEVICENUM,
                                                       POWER_DEVICETYPE,
                                                       USER_TRANSTYPE,
                                                       MESSAGE_TIMEOUT);
                        if (bVerbose)
                            std::cout << "Assigning channel: " << SPD_CDC_ANTCHANNEL << std::endl;


                        pclMessageObject->AssignChannel(SPD_CDC_ANTCHANNEL,
                                                        PARAMETER_TX_NOT_RX,
                                                        0,
                                                        MESSAGE_TIMEOUT);
                    }
                    else if (ucChannelNr == SPD_CDC_ANTCHANNEL) {
                        pclMessageObject->SetChannelID(SPD_CDC_ANTCHANNEL,
                                                       SPD_CDC_DEVICENUM,
                                                       SPD_CDC_DEVICETYPE,
                                                       USER_TRANSTYPE,
                                                       MESSAGE_TIMEOUT);
                    }
                    break;
                }

                case MESG_CHANNEL_ID_ID:
                {
                    if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error configuring channel ID: Code 0x" << std::hex << stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose) {
                        std::cout << "Channel ID set: " << +ucChannelNr << std::endl;
                        std::cout << "Setting Radio Frequency" << std::endl;
                    }
                    if (ucChannelNr == POWER_ANTCHANNEL) {
                        pclMessageObject->SetChannelRFFrequency(POWER_ANTCHANNEL,
                                                                USER_RADIOFREQ,
                                                                MESSAGE_TIMEOUT);
                    }
                    else if (ucChannelNr == SPD_CDC_ANTCHANNEL) {
                        pclMessageObject->SetChannelRFFrequency(SPD_CDC_ANTCHANNEL,
                                                                USER_RADIOFREQ,
                                                                MESSAGE_TIMEOUT);
                    }
                    break;
                }

                case MESG_CHANNEL_RADIO_FREQ_ID:
                {
                    if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error configuring radio frequency: Code 0x" << std::hex << stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose) {
                        std::cout << "Radio frequency set: " << +ucChannelNr << std::endl;
                        std::cout << "Opening channel" << std::endl;
                    }


                    if (ucChannelNr == POWER_ANTCHANNEL) {
                        pclMessageObject->SetChannelPeriod(POWER_ANTCHANNEL, POWER_PERIOD);
                        pclMessageObject->OpenChannel(POWER_ANTCHANNEL, MESSAGE_TIMEOUT);
                        bBroadcastingSpdCdc = TRUE;
                    }

                    else if (ucChannelNr == SPD_CDC_ANTCHANNEL){
                        pclMessageObject->SetChannelPeriod(SPD_CDC_ANTCHANNEL, SPD_CDC_PERIOD);
                        pclMessageObject->OpenChannel(SPD_CDC_ANTCHANNEL, MESSAGE_TIMEOUT);
                        bBroadcastingPower = TRUE;
                    }

                    break;
                }

                case MESG_OPEN_CHANNEL_ID:
                {
                    if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        if (ucChannelNr == SPD_CDC_ANTCHANNEL)
                            bBroadcastingSpdCdc = FALSE;
                        else if (ucChannelNr == POWER_ANTCHANNEL)
                            bBroadcastingPower = FALSE;
                        std::cerr << "Error opening channel: " << +ucChannelNr
                                  << "Code: 0x" << std::dec << +stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose)
                        std::cout << "Channel opened: " << +ucChannelNr << std::endl;
#if defined (ENABLE_EXTENDED_MESSAGES)
                    if (bVerbose)
                        std::cout << "Enabling extended messages" << std::endl;
                    pclMessageObject->RxExtMesgsEnable(TRUE);
#endif
                    break;
                }

                case MESG_RX_EXT_MESGS_ENABLE_ID:
                {
                    if(stMessage.aucData[2] == INVALID_MESSAGE)
                    {
                        std::cerr << "Extended messages not supported in this ANT product" << std::endl;
                        break;
                    }
                    else if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error enabling extended messages: Code " << std::dec << +stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose)
                        std::cout << "Extended messages enabled" << std::endl;
                    break;
                }

                case MESG_UNASSIGN_CHANNEL_ID:
                {
                    if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error unassigning channel: Code " << std::dec << +stMessage.aucData[2] << std::endl;
                        break;
                    }
                    if (bVerbose) {
                        std::cout << "Channel unassigned" << std::endl;
                    }
                    bMyDone = TRUE;
                    break;
                }

                case MESG_CLOSE_CHANNEL_ID:
                {
                    if(stMessage.aucData[2] == CHANNEL_IN_WRONG_STATE)
                    {
                        // We get here if we tried to close the channel after the search timeout (slave)
                        if (bVerbose) {
                            std::cout << "Channel is already closed" << std::endl;
                            std::cout << "Unassigning channel" << std::endl;
                        }
                        if (ucChannelNr == SPD_CDC_ANTCHANNEL)
                            pclMessageObject->UnAssignChannel(SPD_CDC_ANTCHANNEL, MESSAGE_TIMEOUT);
                        else if (ucChannelNr == POWER_ANTCHANNEL)
                            pclMessageObject->UnAssignChannel(POWER_ANTCHANNEL, MESSAGE_TIMEOUT);
                    }
                    else if(stMessage.aucData[2] != RESPONSE_NO_ERROR)
                    {
                        std::cerr << "Error closing channel: Code " <<  std::dec << +stMessage.aucData[2] << std::endl;
                        break;
                    }
                    break;
                }
                case MESG_EVENT_ID:
                {
                    switch(stMessage.aucData[2])
                    {
                        case EVENT_CHANNEL_CLOSED:
                        {
                            if (bVerbose) {
                                std::cout << "Channel closed: " << +ucChannelNr << std::endl;
                                std::cout << "Unassigning channel" << std::endl;
                            }

                            if (ucChannelNr == SPD_CDC_ANTCHANNEL)
                                pclMessageObject->UnAssignChannel(SPD_CDC_ANTCHANNEL, MESSAGE_TIMEOUT);
                            else if (ucChannelNr == POWER_ANTCHANNEL)
                                pclMessageObject->UnAssignChannel(POWER_ANTCHANNEL, MESSAGE_TIMEOUT);
                        }
                        case EVENT_TX:
                        {

                            if (ucChannelNr == POWER_ANTCHANNEL)
                                TransmitPower(ergo->GetPower());
                            else if (ucChannelNr == SPD_CDC_ANTCHANNEL)
                                TransmitSpdCdc(ergo->GetSpeed(), ergo->GetCadence());

                            break;

                        }
                        case EVENT_TRANSFER_TX_FAILED:
                        {
                            if (bVerbose)
                                std::cerr << "Transfer failed" << std::endl;
                            break;
                        }
                        case EVENT_CHANNEL_COLLISION:
                        {
                            if (bVerbose)
                                std::cerr << "Channel collision" << std::endl;
                            break;
                        }
                        default:
                        {
                            if (bVerbose)
                                std::cerr << "Unhandled channel event: 0x" << std::hex << stMessage.aucData[2] << std::endl;
                            break;
                        }

                    }

                    break;
                }

                default:
                {
                    //printf("Unhandled response 0%d to message 0x%X\n", stMessage.aucData[2], stMessage.aucData[1]);
                    break;
                }
            }
            break;
        }

        case MESG_STARTUP_MESG_ID:
        {
            if (bVerbose) {
                std::cout << "Reset complete, reason: ";
                UCHAR ucReason = stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX];
                if(ucReason == RESET_POR)
                    std::cout << "RESET_POR";
                if(ucReason & RESET_SUSPEND)
                    std::cout << "RESET_SUSPEND";
                if(ucReason & RESET_SYNC)
                    std::cout << "RESET_SYNC";
                if(ucReason & RESET_CMD)
                    std::cout << "RESET_CMD";
                if(ucReason & RESET_WDT)
                    std::cout << "RESET_WDT";
                if(ucReason & RESET_RST)
                    std::cout << "RESET_RST";
                std::cout << std::endl;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * Sets up the transmission buffer for transmitting current ergo power data
 * @param power current ergo power to transmit
 */
void AntService::TransmitPower(USHORT power) {
    if (!bBroadcastingPower)
        return;

    cumulativePower += power;

    aucTransmitBuffer[0] = 0x10; // Standard Power-Only message
    aucTransmitBuffer[1] = updateEventCountPower++; // Update event count
    aucTransmitBuffer[2] = 0xFF; // Pedal Power off
    aucTransmitBuffer[3] = 0xFF; // Cadence set invalid
    aucTransmitBuffer[4] = cumulativePower; // Accumulated power LSB
    aucTransmitBuffer[5] = (cumulativePower >> 8) & 0xFF; // Accumulated power MSB
    aucTransmitBuffer[6] = power; // Power LSB
    aucTransmitBuffer[7] = power >> 8; // Power MSB
    pclMessageObject->SendBroadcastData(POWER_ANTCHANNEL, aucTransmitBuffer);
}

/**
 * Sets up the transmission buffer for transmitting current speed and cadence data
 * @param speed current ergo speed to transmit
 * @param cadence current ergo cadence to transmit
 */
void AntService::TransmitSpdCdc(USHORT speed, USHORT cadence) {
    if (!bBroadcastingSpdCdc)
        return;

    if (!nextCdcTransmit && cadence) {
        usCumCranks++;
        // Transforming cadence according to ANT bike power sensor documentation section 6.5.1
        usCrankRevPeriod = (60 * 1024) / cadence;
        usLastCrankEvent += usCrankRevPeriod;
        nextCdcTransmit = lround(4 * ((double) usCrankRevPeriod / (double) 1024));
    }
    if (!nextSpdTransmit && speed) {
        usCumWheelRev++;
        // Transforming cadence according to ANT bike speed/cadence sensor documentation section 5.5.1
        usWheelRevPeriod = 36 * 64 * SPEED_CIRCUMFERENCE / (625 * speed);
        usLastWheelEvent += usWheelRevPeriod;
        nextSpdTransmit = lround(4 * ((double) usWheelRevPeriod / (double) 1024));
    }
    aucTransmitBuffer[MESSAGE_BUFFER_DATA1_INDEX] = usLastCrankEvent; // Last cadence event LSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA2_INDEX] = (usLastCrankEvent >> 8) & 0xFF; // Last cadence event MSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA3_INDEX] = usCumCranks; // Cumulative pedal revolutions LSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA4_INDEX] = usCumCranks >> 8; // Cumulative pedal revolutions MSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA5_INDEX] = usLastWheelEvent; // Last speed event LSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA6_INDEX] = (usLastWheelEvent >> 8) & 0xFF; // Last speed event MSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA7_INDEX] = usCumWheelRev; // Cumulative wheel revolutions LSB
    aucTransmitBuffer[MESSAGE_BUFFER_DATA8_INDEX] = usCumWheelRev >> 8; // Cumulative wheel revolutions MSB

    pclMessageObject->SendBroadcastData(SPD_CDC_ANTCHANNEL, aucTransmitBuffer);
    if (speed)
        nextSpdTransmit--;
    if (cadence)
        nextCdcTransmit--;
}
