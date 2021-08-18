//
// Created by Johan Lövgren on 2021-08-15.
//

#ifndef DAUMERGOANT_DAUMERGO8008TRS_H
#define DAUMERGOANT_DAUMERGO8008TRS_H

#include "DaumErgo.h"
#include "../utils/Serial.h"

#define ARG_8K8TRS "8k8trs"
#define ERGO_8K8TRS_MAX_BUFFER_SIZE 256

/**
 * DaumErgo class for the 8008 TRS bike.
 */
class DaumErgo8008TRS: public DaumErgo {
public:
    /**
     * Default constructor
     * @param verbose output enabled if true
     */
    explicit  DaumErgo8008TRS(bool verbose);
    ~DaumErgo8008TRS();
    /**
     * Initializes the DaumErgo8008TRS, setting up the serial connection
     * @param serialPort the name of the serial port which to connect
     * @return true if successful, otherwise false.
     * If unsuccessful, errno is set accordingly
     */
    bool Init(const char *serialPort) override;
    /**
     * Closes the DaumErgo8008TRS, closing the serial connection and stops the data updater
     */
    void Close() override;
    /**
     * Starts the data updater which updates the data periodically.
     * Runs until Close() is called.
     * @return true if started successfully, otherwise false
     */
    bool RunDataUpdater() override;

private:
    Serial* serial;
    unsigned char GetErgoAddress();
    void DataUpdater();
    void UpdateTrainingData();


    unsigned char ergoAddress;
    u_int8_t currentProgramme, currentUser, pulse, pulseStatus, gear, time;
    float distance, joule, realJoule;
    bool pedalOnOff;
    unsigned char txBuffer[ERGO_8K8TRS_MAX_BUFFER_SIZE];
    unsigned char rxBuffer[ERGO_8K8TRS_MAX_BUFFER_SIZE];
};


#endif //DAUMERGOANT_DAUMERGO8008TRS_H
