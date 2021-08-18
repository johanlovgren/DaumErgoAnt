//
// Created by Johan Lövgren on 2021-08-12.
//

#ifndef DAUMERGOANT_DAUMERGO_H
#define DAUMERGOANT_DAUMERGO_H

#include <mutex>
#include <thread>

/**
 * Superclass for the Daum ergo trainers
 */
class DaumErgo {
public:
    DaumErgo();
    /**
     * Initialize the ergo, setting up connection
     * @return true if successful, otherwise false.
     * If not successful, errno is set accordingly
     */
    virtual bool Init(const char*) = 0;
    /**
     * Starts the data updater which updates the data periodically.
     * Runs until Close() is called.
     * @return true if started successfully, otherwise false.
     */
    virtual bool RunDataUpdater() = 0;
    /**
     * Closes the ergo, closing the connection and stops the data updater.
     */
    virtual void Close() = 0;
    /**
    * Get the current power from the ergo trainer
    * @return current power
    */
    unsigned short GetPower();
    /**
     * Get the current cadence from the ergo trainer
     * @return current cadence
     */
    unsigned short GetCadence();
    /**
     * Get the current speed from the ergo trainer
     * @return current speed
     */
    unsigned short GetSpeed();

protected:
    unsigned short usPower;
    unsigned short usCadence;
    unsigned short usSpeed;
    bool bVerbose; // Used for verbose output
    bool bDone;
    bool bInitialized;
    std::mutex dataMutex;
};



#endif //DAUMERGOANT_DAUMERGO_H
