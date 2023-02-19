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
#include "../../network_key.cfg"


#define ENABLE_EXTENDED_MESSAGES

#define USER_BAUDRATE         (57600)  // For AT3/AP2, use 57600
#define USER_RADIOFREQ        (57) // RF Frequency + 2400

#define USER_NETWORK_NUM      (0)      // The network key is assigned to this network number
#define MESSAGE_TIMEOUT       (1000)

// ----------------------- Public -------------------------------

AntService::AntService(bool verbose) {
    pclSerialObject = (DSISerialGeneric *) nullptr;
    pclMessageObject = (DSIFramerANT *) nullptr;
    uiDSIThread = (DSI_THREAD_ID) nullptr;
    bDone = FALSE;
    initialised = FALSE;

    bVerbose = verbose;

    nInitialisedProfiles = 0;
    currentlyInitialisingProfile = false;
    memset(aucTransmitBuffer, 0, ANT_STANDARD_DATA_PAYLOAD_SIZE);
}

AntService::~AntService() {
    delete pclMessageObject;
    delete pclSerialObject;
}

bool AntService::AddAntProfile(AntProfile *antProfile) {
    if (antProfiles.size() >= MAX_ANT_CHANNELS)
        return false;

    antProfile->SetChannelNumber(antProfiles.size());
    antProfiles.push_back(antProfile);
    return true;
}

BOOL AntService::Init(UCHAR ucUSBDeviceNumber_) {

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
    if (!bStatus) {
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

void AntService::Close() {
    if (!initialised)
        return;

    for (int i = 0; i < antProfiles.size(); ++i) {
        pclMessageObject->CloseChannel(i, MESSAGE_TIMEOUT);
        // TODO Fix sync and remove this dirty solution
        DSIThread_Sleep(500);
    }

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
    if (pclSerialObject)
        pclSerialObject->Close();

#if defined(DEBUG_FILE)
    DSIDebug::Close();
#endif

}

// ----------------------- Private ------------------------------

/**
 * Prints a transmission buffer.
 * @param aucTransmitBuffer buffer to print
 */
void printTransmissionBuffer(UCHAR *aucTransmitBuffer, uint8_t len) {
    for (int i = 0; i < len; ++i) {
        printf("[%02x]", aucTransmitBuffer[i]);
    }
    std::cout << std::endl;
}


/**
 * Initializes the ANT service, resetting system and setting network key
 * @return
 */
BOOL AntService::InitANT() {
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
DSI_THREAD_RETURN AntService::RunMessageThread(void *pvParameter_) {
    ((AntService *) pvParameter_)->MessageThread();
    return nullptr;
}

/**
 * Entry point for message thread. Checks for received messages and processes these
 */
void AntService::MessageThread() {
    ANT_MESSAGE stMessage;
    USHORT usSize;
    bDone = FALSE;

    while (!bDone) {
        if (pclMessageObject->WaitForMessage(1000)) {
            usSize = pclMessageObject->GetMessage(&stMessage);

            if (bDone)
                break;

            if (usSize == DSI_FRAMER_ERROR) {
                // Get the message to clear the error
                pclMessageObject->GetMessage(&stMessage, MESG_MAX_SIZE_VALUE);
                continue;
            }

            if (usSize != DSI_FRAMER_ERROR && usSize != DSI_FRAMER_TIMEDOUT && usSize != 0) {
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
 * Process an event after the network key was set.
 * @param ucChannelNr Channel number associated to the event
 */
void AntService::ProcessNetworkKeyResponse(ANT_MESSAGE stMessage) {
    if (bVerbose) {
        std::cout << "Network key set" << std::endl;
        std::cout << "Assigning channel: " << +nInitialisedProfiles << std::endl;
    }
    pclMessageObject->AssignChannel(nInitialisedProfiles,
                                    antProfiles[nInitialisedProfiles]->GetChannelType(),
                                    USER_NETWORK_NUM,
                                    MESSAGE_TIMEOUT);
    currentlyInitialisingProfile = true;
}

/**
 * Process an event after the channel is assigned
 * @param ucChannelNr Channel number associated to the event
 */
void AntService::ProcessAssignChannelResponse(unsigned char ucChannelNr) {
    if (bVerbose) {
        std::cout << "Channel assigned " << +ucChannelNr << std::endl;
        std::cout << "Setting channel ID" << std::endl;
    }
    if (currentlyInitialisingProfile) {
        pclMessageObject->SetChannelID(nInitialisedProfiles,
                                       antProfiles[nInitialisedProfiles]->GetDeviceNum(),
                                       antProfiles[nInitialisedProfiles]->GetDeviceType(),
                                       antProfiles[nInitialisedProfiles]->GetTransType(),
                                       MESSAGE_TIMEOUT);
    }
}

/**
 * Process an event after the channel ID is set
 * @param ucChannelNr Channel number associated to the event
 */
void AntService::ProcessChannelIDResponse(unsigned char ucChannelNr) {
    if (bVerbose) {
        std::cout << "Channel ID set: " << +ucChannelNr << std::endl;
        std::cout << "Setting Radio Frequency: " << USER_RADIOFREQ << std::endl;
    }

    if (currentlyInitialisingProfile) {
        pclMessageObject->SetChannelRFFrequency(nInitialisedProfiles,
                                                USER_RADIOFREQ,
                                                MESSAGE_TIMEOUT);
    }
}

/**
 * Process an event when the channel frequency is set
 * @param ucChannelNr Channel number associated to the event
 */
void AntService::ProcessChannelRadioFreq(unsigned char ucChannelNr) {
    if (bVerbose) {
        std::cout << "Radio frequency set at channel: " << +ucChannelNr << std::endl;
        std::cout << "Opening channel" << std::endl;
    }

    if (currentlyInitialisingProfile) {
        pclMessageObject->SetChannelPeriod(nInitialisedProfiles,
                                           antProfiles[nInitialisedProfiles]->GetChannelPeriod());
        pclMessageObject->OpenChannel(nInitialisedProfiles,
                                      MESSAGE_TIMEOUT);
        nInitialisedProfiles++;
        currentlyInitialisingProfile = false;
    }
    if (nInitialisedProfiles < antProfiles.size()) {
        pclMessageObject->AssignChannel(nInitialisedProfiles,
                                        antProfiles[nInitialisedProfiles]->GetChannelType(),
                                        USER_NETWORK_NUM,
                                        MESSAGE_TIMEOUT);
        currentlyInitialisingProfile = true;
    }
}

/**
 * Process a message event
 * @param stMessage ANT message containing a message event
 * @param ucChannelNr Associated channel number to the message event
 */
void AntService::ProcessMessageEvent(ANT_MESSAGE stMessage, unsigned char ucChannelNr) {
    switch (stMessage.aucData[2]) {
        case EVENT_CHANNEL_CLOSED: {
            if (bVerbose) {
                std::cout << "Channel closed: " << +ucChannelNr << std::endl;
                std::cout << "Unassigning channel" << std::endl;
            }
            pclMessageObject->UnAssignChannel(ucChannelNr, MESSAGE_TIMEOUT);

        }
        case EVENT_TX: {
            antProfiles[ucChannelNr]->HandleTXEvent(aucTransmitBuffer);
            pclMessageObject->SendBroadcastData(ucChannelNr, aucTransmitBuffer);
            break;

        }
        case EVENT_TRANSFER_TX_FAILED: {
            if (bVerbose)
                std::cerr << "Transfer failed" << std::endl;
            break;
        }
        case EVENT_CHANNEL_COLLISION: {
            if (bVerbose)
                std::cerr << "Channel collision" << std::endl;
            break;
        }
        default: {
            if (bVerbose)
                std::cerr << "Unhandled channel event: 0x" << std::hex << stMessage.aucData[2]
                          << std::endl;
            break;
        }
    }
}

/**
 * Process a response event.
 * @param stMessage Message received that is a response event.
 */
void AntService::ProcessResponseEvent(ANT_MESSAGE stMessage) {
    UCHAR ucChannelNr = pclMessageObject->GetChannelNumber(&stMessage);
    //RESPONSE TYPE
    switch (stMessage.aucData[1]) {
        case MESG_NETWORK_KEY_ID: {
            if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error configuring network key: Code " << stMessage.aucData[2] << std::endl;
                break;
            }
            ProcessNetworkKeyResponse(stMessage);
            break;
        }
        case MESG_ASSIGN_CHANNEL_ID: {
            if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error assigning channel: Code " << stMessage.aucData[2] << std::endl;
                break;
            }
            ProcessAssignChannelResponse(ucChannelNr);
            break;
        }
        case MESG_CHANNEL_ID_ID: {
            if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error configuring channel ID: Code 0x" << std::hex << stMessage.aucData[2]
                          << std::endl;
                break;
            }
            ProcessChannelIDResponse(ucChannelNr);
            break;
        }
        case MESG_CHANNEL_RADIO_FREQ_ID: {
            if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error configuring radio frequency: Code 0x" << std::hex << stMessage.aucData[2]
                          << std::endl;
                break;
            }
            ProcessChannelRadioFreq(ucChannelNr);
            break;
        }
        case MESG_OPEN_CHANNEL_ID: {
            if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
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
        case MESG_EVENT_ID: {
            ProcessMessageEvent(stMessage, ucChannelNr);
            break;
        }
        case MESG_RX_EXT_MESGS_ENABLE_ID: {
            if (stMessage.aucData[2] == INVALID_MESSAGE) {
                std::cerr << "Extended messages not supported in this ANT product" << std::endl;
                break;
            } else if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error enabling extended messages: Code " << std::dec << +stMessage.aucData[2]
                          << std::endl;
                break;
            }
            if (bVerbose)
                std::cout << "Extended messages enabled" << std::endl;
            break;
        }

        case MESG_UNASSIGN_CHANNEL_ID: {
            if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error unassigning channel: Code " << std::dec << +stMessage.aucData[2]
                          << std::endl;
                break;
            }
            if (bVerbose) {
                std::cout << "Channel unassigned" << std::endl;
            }
            break;
        }

        case MESG_CLOSE_CHANNEL_ID: {
            if (stMessage.aucData[2] == CHANNEL_IN_WRONG_STATE) {
                // We get here if we tried to close the channel after the search timeout (slave)
                if (bVerbose) {
                    std::cout << "Channel is already closed" << std::endl;
                    std::cout << "Unassigning channel" << std::endl;
                }
                pclMessageObject->UnAssignChannel(ucChannelNr, MESSAGE_TIMEOUT);
            } else if (stMessage.aucData[2] != RESPONSE_NO_ERROR) {
                std::cerr << "Error closing channel: Code " << std::dec << +stMessage.aucData[2] << std::endl;
                break;
            }
            break;
        }
        default: {
            break;
        }
    }
}

/**
 * Process the received Acknowledged message
 * @param stMessage ANT_MESSAGE associated to the acknowledged message
 */
void AntService::ProcessAcknowledgedEvent(ANT_MESSAGE stMessage) {
    UCHAR ucChannelNr = pclMessageObject->GetChannelNumber(&stMessage);
    if (ucChannelNr >= antProfiles.size())
        return;
    uint8_t nResponses = antProfiles[ucChannelNr]->HandleAckEvent(&stMessage.aucData[1], aucTransmitBuffer);
    if (nResponses == NOT_SUPPORTED) // Ignores if not supported
        return;

    // May be sending responses too fast
    for (int i = 0; i < nResponses; ++i) {
        pclMessageObject->SendBroadcastData(ucChannelNr, aucTransmitBuffer);
        std::cout << "Sent" << std::endl;
    }
}

/**
 * Process the received messages, including event messages and thus ensures that channels are initialized properly
 * @param stMessage message received
 */
void AntService::ProcessMessage(ANT_MESSAGE stMessage) {
    switch (stMessage.ucMessageID) {
        //RESPONSE MESG
        case MESG_RESPONSE_EVENT_ID: {
            ProcessResponseEvent(stMessage);
            break;
        }
        case MESG_ACKNOWLEDGED_DATA_ID: {
            ProcessAcknowledgedEvent(stMessage);
            break;
        }
        case MESG_STARTUP_MESG_ID: {
            if (bVerbose) {
                std::cout << "Reset complete, reason: ";
                UCHAR ucReason = stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX];
                if (ucReason == RESET_POR)
                    std::cout << "RESET_POR";
                if (ucReason & RESET_SUSPEND)
                    std::cout << "RESET_SUSPEND";
                if (ucReason & RESET_SYNC)
                    std::cout << "RESET_SYNC";
                if (ucReason & RESET_CMD)
                    std::cout << "RESET_CMD";
                if (ucReason & RESET_WDT)
                    std::cout << "RESET_WDT";
                if (ucReason & RESET_RST)
                    std::cout << "RESET_RST";
                std::cout << std::endl;
            }
            break;
        }
        default: {
            break;
        }
    }
}