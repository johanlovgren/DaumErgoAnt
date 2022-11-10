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
     * Sets the target power of the Ergo
     * @param power New target power in Watt
     */
    virtual bool SetPower(uint16_t power) = 0;

    /**
     * Sets the target resistance of the Ergo.
     * @param resistance New target resistance as percent of the maximum resistance supported by the Ergo.
     * Must be a integer between 0 and 100, if higher, 100 is assumed.
     */
    virtual bool SetResistance(uint8_t resistance) = 0;

    /**
    * Get the current power from the ergo trainer
    * @return current power
    */
    unsigned short GetPower();

    /**
     * Get the currently accumulated power from when training started. Rollover at 65535.
     * @return Currently accumulated power
     */
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

    /**
     * Getter of ergo trainer equipment type.
     * See page 29 in ANT FEC documentation for more information
     * @return The ergo trainer equipment type
     */
    uint8_t GetEquipmentType() const;

    /**
     * Getter of currently elapsed time from the ergo trainer
     * @return Currently accumulated time from the ergo trainer, in seconds.
     */
    uint16_t GetElapsedTime();

    /**
     * Getter of distance traveled on the ergo trainer
     * @return Accumulated distance traveled in unit of 100m
     */
    uint16_t GetDistanceTraveled();

    /**
     * Getter of heart rate recorded by ergo trainer
     * @return Current heart rate from ergo, in bp/m
     */
    uint8_t GetHeartRate() const;

    /**
     * Getter of ergo trainer capabilities in terms of heart rate, distance and speed.
     * See page 31 in ANT FEC documentation for more information
     * @return Number representing capability bits
     */
    uint8_t GetHrDistanceSpeedCapabilitiesBits() const;

    /**
     * Getter of ergo trainer capabilities in terms of training mode.
     * See page 60 in ANT FEC documentation for more information
     * @return Number representing capability bits
     */
    uint8_t GetTrainingModeCapabilities() const;

    /**
     * Getter of cycle length of the ergo trainer. Cycle length is defined differently depending on ergo trainer type.
     * See page 33 in ANT FEC documentation for more information.
     * @return The ergo trainers cycle length
     */
    virtual uint8_t GetCycleLength() = 0;

    /**
     * Getter of the current resistance level in percent of maximum resistance based on the ergo trainers maximum
     * resistance.
     * @return Percent of the current resistance of the ergo trainer.
     */
    virtual uint8_t GetResistanceLevel() = 0;

    /**
     * Getter of the current ergo trainers incline.
     * @return Current incline
     */
    virtual uint16_t GetIncline() = 0;

    /**
     * Getter of the ergo trainers fitness equipment state bits,
     * See page 32 in ANT FEC documentation for more information.
     * @return Number representing ergo trainers FE state bits
     */
    uint8_t GetFEStateBits() const;

    /**
     * Getter of the ergo trainer target power flag. Target power flag indicates whether the current power is reached
     * or not. For instance, too low cadence can result in incorrect power.
     * See page 47 in ANT FEC documentation for more information.
     * @return Number representing ergo trainer power flag bits
     */
    uint8_t GetTargetPowerFlag();

    /**
     * Getter of the ergo trainer power status bit field. Indicates whether the ergo trainer needs power calibration
     * or not.
     * See page 46 in ANT FEC documentation for more information.
     * @return
     */
    uint8_t GetTrainerPowerStatusBitField();

    /**
     * Getter of the ergo trainer power event counter. Power event counter shall be increased on every received power
     * data update from ergo trainer.
     * See page 45 in ANT FEC documentation for more information.
     * @return Number of power updates, rollover 255.
     */
    uint8_t GetPowerEventCounter();

    /**
     * Getter of the currently set power target on the ergo trainer
     * @return Currently set power target in Watt
     */
    uint16_t GetTargetPower() const;

    /**
     * Getter of the currently set resistance target on the ergo trainer.
     * @return Currently set resistance level in %
     */
    uint8_t GetTargetResistance() const;

protected:
    unsigned short currentPower;
    unsigned short currentCadence;
    uint16_t elapsedTime, distanceTraveled, currentSpeed, accumulatedPower, targetPower;
    uint8_t equipmentType, heartRate, hrDistanceSpeedCapabilitiesBits, updatePowerEventCount,
    trainerPowerStatusBitField, feStateBits, targetPowerFlag, trainingModCapabilities, targetResistance;
    bool verbose; // Used for verbose output
    bool done;
    bool initialized;
    std::mutex dataMutex;
};

#endif //DAUMERGOANT_DAUMERGO_H
