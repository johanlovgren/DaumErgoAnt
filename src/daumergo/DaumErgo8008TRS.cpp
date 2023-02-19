//
// Created by Johan Lövgren on 2021-08-15.
//
#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include "DaumErgo8008TRS.h"


// Serial settings according to Daum documentation
#define BAUD_RATE 9600

#define GET_TRAINING_DATA 0x40
#define GET_ADDRESS 0x11
#define SET_WATT 0x51
#define SET_TIME 0x62

#define SET_WATT_BYTES 3
#define SET_TIME_BYTES 5

// Training data offsets according to Daum documentation
#define PROGRAMME_OFFSET 2
#define USER_OFFSET 3
#define PEDAL_ON_OFF_OFFSET 4
#define POWER_OFFSET 5
#define CADENCE_OFFSET 6
#define SPEED_OFFSET 7
#define DISTANCE_OFFSET 8
#define TIME_OFFSET 10
#define JOULE_OFFSET 12
#define PULSE_OFFSET 14
#define PULSE_STATUS_OFFSET 15
#define GEAR_OFFSET 16
#define REAL_JOULE_OFFSET 17

#define TIME 0
#define WATT 1
#define WORKOUT_START_SLEEP 10

#define ANT_EQUIPMENT_TYPE 25
#define ANT_CAPABILITIES_STATE 52 // May need to be changed
#define ANT_FE_BIT_FIELD 3
#define CYCLE_CIRCUMFERENCE 210 // The circumference of the wheel for speed calculation
#define MAXIMUM_WATT 800
#define MINIMUM_WATT 25

#define FE_STATE_READY 2
#define FE_STATE_IN_USE 3
#define FE_STATE_PAUSED 4

// ----------------------- Public -------------------------------

DaumErgo8008TRS::DaumErgo8008TRS(bool verbose) {
    serial = nullptr;
    this->verbose = verbose;
    ergoAddress = -1;
    equipmentType = ANT_EQUIPMENT_TYPE;
    hrDistanceSpeedCapabilitiesBits = 0;
    updatePowerEventCount = 0;
    feStateBits = 2; // Set to READY
    trainerPowerStatusBitField = 0; // Calibration complete/not required
    targetPowerFlag = 0; // Trainer operating at the target power, or no target power set.
    elapsedTime = 0;
    distanceTraveled = 0;
    currentSpeed = 0;
    accumulatedPower = 0;
    heartRate = 0xFF; // Invalid
    // Training mode capabilities: Bit 0 Basic resistance, bit 1 target power, bit 2 sim mode, bit 3-7 set 0
    trainingModCapabilities = 1 + (1 << 1);
}

DaumErgo8008TRS::~DaumErgo8008TRS() {
    delete serial;
}

bool DaumErgo8008TRS::Init(const char *serialPort) {
    if (verbose)
        std::cout << "Trying to open port: " << serialPort << std::endl;
    serial = new Serial(serialPort, BAUD_RATE);
    if (!serial->Open()) {
        std::cerr << "Failed to open port at: " << serialPort << " Error: " << strerror(errno) << std::endl;
        return false;
    }
    if (!GetErgoAddress()) {
        std::cerr << "Failed to fetch Ergo address at port: " << serialPort << std::endl;
        return false;
    }
    if (verbose) {
        std::cout << "Ergo address fetched: 0x" << std::hex << +ergoAddress << std::endl;
    }
    return true;
}

void DaumErgo8008TRS::Close() {
    if (!serial->IsOpen()) {
        return;
    }

    done = true;
    if (verbose)
        std::cout << "Closing serial port" << std::endl;
    serial->Close();
}

void DaumErgo8008TRS::DataUpdater() {
    while (!done) {
        UpdateTrainingData();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool DaumErgo8008TRS::RunDataUpdater() {
    if (!serial->IsOpen()) {
        std::cerr << "Serial port " << serial->GetPortName() << " is not open" << std::endl;
        return false;
    }
    done = false;
    std::thread(&DaumErgo8008TRS::DataUpdater, this).detach();
    return true;
}

bool DaumErgo8008TRS::RunWorkout(std::vector<std::tuple<int, int>> time_watt) {
    if (!serial->IsOpen()) {
        std::cerr << "Serial port " << serial->GetPortName() << " is not open" << std::endl;
        std::cerr << "Failed to start workout" << std::endl;
        return false;
    }
    std::thread(&DaumErgo8008TRS::SimpleController, this, time_watt).detach();
    return true;
}


/**
 * Returns the cycle length, in this case, wheel circumference
 * @return
 */
uint8_t DaumErgo8008TRS::GetCycleLength() {
    return CYCLE_CIRCUMFERENCE;
}

uint8_t DaumErgo8008TRS::GetResistanceLevel() {
    dataMutex.lock();
    uint16_t currentWatt = currentPower;
    dataMutex.unlock();
    return ((currentWatt - MINIMUM_WATT) / (MAXIMUM_WATT - MINIMUM_WATT)) * 100;
}

/**
 * Returns the current incline
 * @return 0x7FFF for invalid
 */
uint16_t DaumErgo8008TRS::GetIncline() {
    return 0x7FFF;
}

bool DaumErgo8008TRS::SetPower(uint16_t power) {
    targetPower = power;
    power = (uint16_t) (round(power / 5) * 5);
    if (power > MAXIMUM_WATT)
        SetWatt(MAXIMUM_WATT);
    else
        SetWatt(power);
    return true;
}

bool DaumErgo8008TRS::SetResistance(uint8_t resistance) {
    // Resistance is disabled due to Ergo issues
    return false;
    resistance = resistance >= 100 ? 100 : resistance;
    targetResistance = resistance;
    uint16_t power = MAXIMUM_WATT * ((double) resistance / 100);
    SetPower(power);
    return true;
}

// ----------------------- Private  -------------------------------

void DaumErgo8008TRS::SetTime(uint8_t seconds, uint8_t minutes, uint8_t hours) {
    serialMutex.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    memset(txBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    memset(rxBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    // Three bytes to write: query, address and new Watt (divided by 5)
    txBuffer[0] = SET_TIME;
    txBuffer[1] = ergoAddress;
    txBuffer[2] = seconds;
    txBuffer[3] = minutes;
    txBuffer[4] = hours;
    serial->Write(txBuffer, SET_TIME_BYTES);
    // Read to clear
    serial->Read(rxBuffer, 2);
    serialMutex.unlock();
}

void DaumErgo8008TRS::SetWatt(int watt) {
    serialMutex.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    memset(txBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    memset(rxBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    // Three bytes to write: query, address and new Watt (divided by 5)
    txBuffer[0] = SET_WATT;
    txBuffer[1] = ergoAddress;
    txBuffer[2] = (uint8_t) (watt / 5);
    serial->Write(txBuffer, SET_WATT_BYTES);
    // Read to clear
    serial->Read(rxBuffer, 3);
    serialMutex.unlock();
}

void DaumErgo8008TRS::SimpleController(std::vector<std::tuple<int, int>> time_watt) {
    std::this_thread::sleep_for(std::chrono::seconds(WORKOUT_START_SLEEP));
    for (int i = 0; i < time_watt.size() && !done; ++i) {
        SetWatt(std::get<WATT>(time_watt[i]));
        std::this_thread::sleep_for(std::chrono::seconds(std::get<TIME>(time_watt[i])));
    }
}


/**
 * Fetches the current training data from the ergo and updates the object
 */
void DaumErgo8008TRS::UpdateTrainingData() {
    serialMutex.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    memset(txBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    memset(rxBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    txBuffer[0] = GET_TRAINING_DATA;
    txBuffer[1] = ergoAddress;
    // Two bytes to write, query and address
    serial->Write(txBuffer, 2);
    // Ergo returns 19 bytes of data
    serial->Read(rxBuffer, 19);
    // If data received is not correct, don't update stored data
    if (rxBuffer[0] != GET_TRAINING_DATA) {
        serialMutex.unlock();
        return;
    }
    dataMutex.lock();
    ++updatePowerEventCount;
    currentProgramme = rxBuffer[PROGRAMME_OFFSET];
    currentUser = rxBuffer[USER_OFFSET];
    currentCadence = rxBuffer[CADENCE_OFFSET];
    currentPower = currentCadence ? rxBuffer[POWER_OFFSET] * 5 : 0;
    accumulatedPower += currentPower;
    currentSpeed = rxBuffer[SPEED_OFFSET];
    pulse = rxBuffer[PULSE_OFFSET];
    gear = rxBuffer[GEAR_OFFSET];
    pedalOnOff = rxBuffer[PEDAL_ON_OFF_OFFSET];
    feStateBits = pedalOnOff ? FE_STATE_IN_USE : FE_STATE_IN_USE; // TODO Fix!
    distanceTraveled = rxBuffer[DISTANCE_OFFSET] + (rxBuffer[DISTANCE_OFFSET + 1] << 8);
    elapsedTime = rxBuffer[TIME_OFFSET] + (rxBuffer[TIME_OFFSET + 1] << 8);
    dataMutex.unlock();

    serialMutex.unlock();
}

/**
 * Fetches the bike address which is needed for all other queries
 * @return If successful, the bike address. On failure -1 is returned
 */
bool DaumErgo8008TRS::GetErgoAddress() {
    serialMutex.lock();
    memset(txBuffer, 0, ERGO_8K8TRS_MAX_BUFFER_SIZE);
    txBuffer[0] = GET_ADDRESS;
    if (serial->Write(txBuffer, 1) < 0)
        return false;
    if (serial->Read(rxBuffer, 2) < 0)
        return false;
    ergoAddress = rxBuffer[1];
    serialMutex.unlock();
    return true;
}