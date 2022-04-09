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
     * Runs a workout on the ergo.
     * @param workout Workout to run, where each element in the vector is a tuple defining how long time a certaing
     * wattage should hold
     * @return
     */
    virtual bool RunWorkout(std::vector<std::tuple<int, int>> workout) = 0;
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

    // TODO Continue here!
    virtual uint8_t GetEquipmentType() = 0;
    virtual uint16_t GetElapsedTime() = 0;
    virtual uint16_t GetDistanceTraveled() = 0;
    virtual uint8_t GetHeartRate() = 0;
    virtual uint8_t GetCapabilitiesState() = 0;
    virtual uint8_t GetCycleLength() = 0;
    virtual uint8_t GetResistanceLevel() = 0;
    virtual uint16_t GetIncline() = 0;
    virtual uint8_t GetFEStateBits() = 0;
    virtual uint8_t GetTargetPowerFlag() = 0;
    virtual uint8_t GetTrainerStatusBitField() = 0; // Table 8-27

protected:
    unsigned short currentPower;
    unsigned short currentCadence;
    unsigned short currentSpeed;
    unsigned short distanceTraveled;
    unsigned short elapsedTime;
    bool verbose; // Used for verbose output
    bool done;
    bool initialized;
    std::mutex dataMutex;
};



#endif //DAUMERGOANT_DAUMERGO_H
