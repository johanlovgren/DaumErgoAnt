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

    uint16_t GetAccumulatedPower();

    /**
     * Get the current cadence from the ergo trainer
     * @return current cadence
     */
    uint8_t GetCadence();
    /**
     * Get the current speed from the ergo trainer
     * @return current speed
     */
    uint16_t GetSpeed();



    // TODO Continue here!
    uint8_t GetEquipmentType() const;
    uint16_t GetElapsedTime();
    uint16_t GetDistanceTraveled();
    uint8_t GetHeartRate() const;
    uint8_t GetCapabilitiesBits() const;
    virtual uint8_t GetCycleLength() = 0;
    virtual uint8_t GetResistanceLevel() = 0;
    virtual uint16_t GetIncline() = 0;
    uint8_t GetFEStateBits() const;
    uint8_t GetTargetPowerFlag();
    uint8_t GetTrainerPowerStatusBitField(); // Table 8-27

    uint8_t GetPowerEventCounter();

protected:
    unsigned short currentPower;
    unsigned short currentCadence;
    uint16_t elapsedTime, distanceTraveled, currentSpeed, accumulatedPower;
    uint8_t equipmentType, heartRate, capabilitiesBits, updatePowerEventCount,
    trainerPowerStatusBitField, feStateBits, targetPowerFlag;
    bool verbose; // Used for verbose output
    bool done;
    bool initialized;
    std::mutex dataMutex;
};



#endif //DAUMERGOANT_DAUMERGO_H
