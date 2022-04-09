//
// Created by Johan Lövgren on 2022-03-30.
//

#include "AntFECProfile.h"


#define MASTER 0x10
#define RF_CHANNEL_FREQUENCY 57
#define DEVICE_TYPE 0x11
#define TRANSMISSION_TYPE_MSN 0
#define TRANSMISSION_TYPE_LSN 5
#define DEVICE_NUMBER 666
#define CHANNEL_PERIOD 8192
#define PAIRING 0

#define GENERAL_MAIN_DATA_PAGES_INTERVAL 5
#define FE_SPECIFIC_DATA_PAGES_INTERVAL 5
#define BACKGROUND_DATA_PAGE_80_INTERVAL 66 // Send 2!
#define BACKGROUND_DATA_PAGE_81_INTERVAL 132 // Send 2!

// Data Page 16 - General FE Data
#define DATA_PAGE_16_DATA_PAGE_NUMBER_INDEX 0
#define DATA_PAGE_16_EQUIPMENT_TYPE_INDEX 1
#define DATA_PAGE_16_ELAPSED_TIME_INDEX 2 // Accumulated with rollover every 64 second
#define DATA_PAGE_16_DISTANCE_TRAVELED_INDEX 3 // Accumulated with rollover every 256 meter
#define DATA_PAGE_16_SPEED_LSB_INDEX 4 // Instantaneous speed sent in units of 0.001 m/s
#define DATA_PAGE_16_SPEED_MSB_INDEX 5
#define DATA_PAGE_16_HEART_RATE_INDEX 6 // May be set to invalid, 0xFF
#define DATA_PAGE_16_CAPABILITIES_FE_STATE_INDEX 7

#define DATA_PAGE_16_ELAPSED_TIME_ROLLOVER 64
#define DATA_PAGE_16_DISTANCE_TRAVELED_ROLLOVER 256

#define BIKE_TRAINER_EQUIPMENT_TYPE_BIT_FIELD 25

// Shows that HR is invalid, Distance is OK and Virtual Speed is Invalid
// TODO MOVE! Should not be here.
#define GENERAL_MAIN_DATA_ERGO_8K8TRS_CAPABILITIES 4
// TODO MOVE! CANOT BE CONSTANT!
#define GENERAL_MAIN_DATA_ERGO_8K8TRS_FE_STATE

// Data Page 17 - General Settings Page
#define DATA_PAGE_17_PAGE_NUMBER_INDEX 0
#define DATA_PAGE_17_CYCLE_LENGTH_INDEX 3 // Length of one "cycle", trainer = wheel circumference
#define DATA_PAGE_17_INCLINE_LSB_INDEX 4 // Percentage between -100% and +100%, 0xFF for invalid
#define DATA_PAGE_17_INCLINE_MSB_INDEX 5
#define DATA_PAGE_17_RESISTANCE_LEVEL_INDEX 6 // Percentage of max resistance of equipment 0-100% with a 0.5% unit
#define DATA_PAGE_17_CAPABILITIES_FE_STATE_INDEX 7

// Data Page 25 - Specific Trainer/Stationary Bike Data
#define DATA_PAGE_25_DATA_PAGE_NUMBER_INDEX 0
#define DATA_PAGE_25_UPDATE_EVENT_COUNT_INDEX 1 // Event counter increments with each information update
#define DATA_PAGE_25_INSTANTANEOUS_CADENCE_INDEX 2 // Crank cadence, 0-254 RPM
#define DATA_PAGE_25_ACCUMULATED_POWER_LSB_INDEX 3 // Accumulated power, 1 watt-resolution, 65536W rollover
#define DATA_PAGE_25_ACCUMULATED_POWER_MSB_INDEX 4
#define DATA_PAGE_25_INSTANTANEOUS_POWER_LSB_INDEX 5 // Instantaneous power, 0-4094W
#define DATA_PAGE_25_INSTANTANEOUS_POWER_MSB_BIT_STATUS_INDEX 6 // Bits 0-3 instantaneous power MSB, bits 4-7 trainer status
#define DATA_PAGE_25_FLAG_FE_STATE_INDEX 7 // Bits 0-3 flag bits, bits 4-7 FE State bits

// TODO Move this!
#define DATA_PAGE_25_ERGO_8K8TRS_TRAINER_STATUS 0
// TODO CANT BE CONSTANT, AND SHOULD BE MOVED!
#define DATA_PAGE_25_ERGO_8K8TRS_FLAG_BITS 0

// Specific Trainer Torque Data
#define DATA_PAGE_26_DATA_PAGE_NUMBER_INDEX 0
#define DATA_PAGE_26_UPDATE_EVENT_COUNT_INDEX 1 // Increment with each information update
#define DATA_PAGE_26_WHEEL_TICKS_INDEX 2 // Wheel tick count increments with each wheel revolution
#define DATA_PAGE_26_WHEEL_PERIOD_LSB_INDEX 3 // Accumulated wheel period (updated each event)
#define DATA_PAGE_26_WHEEL_PERIOD_MSB_INDEX 4
#define DATA_PAGE_26_ACCUMULATED_TORQUE_LSB_INDEX 5 // (Accumulated torque)
#define DATA_PAGE_26_ACCUMULATED_TORQUE_MSB_INDEX 6
#define DATA_PAGE_26_CAPABILITIES_FE_STATE_INDEX 7

// Control Data Pages
// Data Page 48 - Basic Resistance
#define DATA_PAGE_48_DATA_PAGE_NUMBER_INDEX 0
#define DATA_PAGE_48_TOTAL_RESISTANCE_INDEX 7 // Percentage of maximum resistance to be applied

// Data Page 49 - Target Power
#define DATA_PAGE_49_DATA_PAGE_NUMBER_INDEX 0
#define DATA_PAGE_49_TARGET_POWER_LSB_INDEX 6 // The target power for a controllable trainer operating in target power mode
#define DATA_PAGE_49_TARGET_POWER_MSB_INDEX 7

// Data Page 50 - Wind Resistance
#define DATA_PAGE_50_DATA_PAGE_NUMBER_INDEX 0 // 50 (0x32)
#define DATA_PAGE_50_WIND_RESISTANCE_COEFFICIENT_INDEX 5 // Product of Frontal Surface Area, Drag and Air density
#define DATA_PAGE_50_WIND_SPEED_INDEX 6 // Speed of simulated wind acting on the cyclist
#define DATA_PAGE_50_WIND_DRAFTING_FACTOR_INDEX 7

// Data Package 51 - Track Resistance
#define DATA_PAGE_51_DATA_PAGE_NUMBER_INDEX 0 // 51 (0x33)
#define DATA_PAGE_51_GRADE_LSB_INDEX 5 // Grade of simulated track
#define DATA_PAGE_51_GRADE_MSB_INDEX 6
#define DATA_PAGE_51_ROLLING_RESISTANCE_COEFFICIENT_INDEX 7 // Coefficient of rolling resistance between bicycle tire and track

// Data Page 54 - FE Capabilities
#define DATA_PAGE_54_DATA_PAGE_NUMBER_INDEX 0 // 54 (0x36)
#define DATA_PAGE_54_MAXIMUM_RESISTANCE_LSB_INDEX 5 // Max applicable resistance of the trainer
#define DATA_PAGE_54_MAXIMUM_RESISTANCE_MSB_INDEX 6
#define DATA_PAGE_54_CAPABILITIES_INDEX 5 // Refer to table 8-46 in documentation

// Data Page 55 - User Configuration
#define DATA_PAGE_55_DATA_PAGE_NUMBER_INDEX 0 // 55 (0x37)
#define DATA_PAGE_55_BICYCLE_WHEEL_DIAMETER_OFFSET_WEIGHT_INDEX 4 // Bit 0-3 Bike wheel diameter offset, 4-7 weight LSN
#define DATA_PAGE_55_BICYCLE_WEIGHT_MSB_INDEX 5 // MSB of bicycle weight entered on the display (0xFF Invalid)
#define DATA_PAGE_55_BICYCLE_WHEEL_DIAMETER_INDEX 6 // Bicycle wheel diameter entered on the display (0xFF Invalid)
#define DATA_PAGE_55_GEAR_RATIO_INDEX 7 // Front:Back Gear Ratio entered on the display (0x00 Invalid)

// Required Common Pages
// Common Page 80 - Manufacturer's Identification
#define COMMON_DATA_PAGE_80_DATA_PAGE_NUMBER_INDEX 0
#define COMMON_DATA_PAGE_80_HW_REVISION_INDEX 3
#define COMMON_DATA_PAGE_80_MANUFACTURER_ID_LSB_INDEX 4
#define COMMON_DATA_PAGE_80_MANUFACTURER_ID_MSB_INDEX 5
#define COMMON_DATA_PAGE_80_MODEL_NUMBER_LSB_INDEX 6
#define COMMON_DATA_PAGE_80_MODEL_NUMBER_MSB_INDEX 7

#define COMMON_DATA_PAGE_81_DATA_PAGE_NUMBER_INDEX 0
#define COMMON_DATA_PAGE_81_SW_REVISION_INDEX 1
#define COMMON_DATA_PAGE_81_SERIAL_NUMBER_0_7_INDEX 4
#define COMMON_DATA_PAGE_81_SERIAL_NUMBER_8_15_INDEX 5
#define COMMON_DATA_PAGE_81_SERIAL_NUMBER_16_23_INDEX 6
#define COMMON_DATA_PAGE_81_SERIAL_NUMBER_24_31_INDEX 7

// Common Page 70 - Command Status
#define COMMON_PAGE_71_DATA_PAGE_NUMBER_INDEX 0 // 71 (0x47)
#define COMMON_PAGE_71_LAST_RECEIVED_COMMAND_ID_INDEX 1 // Indicates data page number of the last control page received
#define COMMON_PAGE_71_SEQUENCE_NUMBER_INDEX 2 // Sequence number used by slave in last received command sequence
#define COMMON_PAGE_71_COMMAND_STATUS_INDEX 3

#define TRANSMIT_PATTERN_REPEATING_DATA_PAGES 64
#define TRANSMIT_PATTERN_COMMON_PAGES 2

AntFECProfile::AntFECProfile(DaumErgo *ergo, DSIFramerANT *antMessageObject, uint8_t channelNumber) {
    this->ergo = ergo;
    this->antMessageObject = antMessageObject;
    this->channelNumber = channelNumber;
    transmitPatternCounter = transmitCommonPageCounter = eventCounter = accumulatedPower = 0;
    memset(txBuffer, 0, ANT_STANDARD_DATA_PAYLOAD_SIZE);
}

AntFECProfile::~AntFECProfile() {
    this->ergo = nullptr;
    this->antMessageObject = nullptr;
}

void AntFECProfile::HandleTXEvent() {
    TransmitPatternA(txBuffer);
    antMessageObject->SendBroadcastData(channelNumber, txBuffer);
}

void AntFECProfile::TransmitPatternA(uint8_t *buf) {
    if (transmitPatternCounter++ < TRANSMIT_PATTERN_REPEATING_DATA_PAGES) {
        DataPage16(buf);
    } else if (transmitCommonPageCounter < TRANSMIT_PATTERN_COMMON_PAGES) {
        DataPage80(buf);
        transmitCommonPageCounter++;
        transmitPatternCounter = transmitCommonPageCounter == 2 ? 0 : transmitPatternCounter;
    } else if (transmitCommonPageCounter < TRANSMIT_PATTERN_COMMON_PAGES * 2) {
        DataPage81(buf);
        transmitCommonPageCounter = ++transmitCommonPageCounter % 4;
        transmitPatternCounter = transmitCommonPageCounter == 2 ? 0 : transmitPatternCounter;
    }
}

void AntFECProfile::DataPage16(uint8_t *buf) {
    unsigned short speed = ergo->GetSpeed();
    buf[DATA_PAGE_16_DATA_PAGE_NUMBER_INDEX] = 16;
    buf[DATA_PAGE_16_EQUIPMENT_TYPE_INDEX] = ergo->GetEquipmentType();
    // Elapsed time have 0.25s as unit and is an accumulated value
    buf[DATA_PAGE_16_ELAPSED_TIME_INDEX] = (ergo->GetElapsedTime() % DATA_PAGE_16_ELAPSED_TIME_ROLLOVER) * 4;
    // Distance traveled have meter unit and is an accumulated value
    buf[DATA_PAGE_16_DISTANCE_TRAVELED_INDEX] = ergo->GetDistanceTraveled() % DATA_PAGE_16_DISTANCE_TRAVELED_ROLLOVER;
    buf[DATA_PAGE_16_SPEED_LSB_INDEX] = (uint8_t) speed;
    buf[DATA_PAGE_16_SPEED_MSB_INDEX] = (uint8_t) (speed >> 8);
    buf[DATA_PAGE_16_HEART_RATE_INDEX] = ergo->GetHeartRate();
    buf[DATA_PAGE_16_CAPABILITIES_FE_STATE_INDEX] = ergo->GetCapabilitiesState();
}
void AntFECProfile::DataPage17(uint8_t *buf) {
    uint16_t incline = ergo->GetIncline();
    buf[DATA_PAGE_17_PAGE_NUMBER_INDEX] = 17;
    buf[DATA_PAGE_17_CYCLE_LENGTH_INDEX] = ergo->GetCycleLength();
    buf[DATA_PAGE_17_INCLINE_LSB_INDEX] = (uint8_t) incline;
    buf[DATA_PAGE_17_INCLINE_MSB_INDEX] = (uint8_t) (incline >> 8);
    buf[DATA_PAGE_17_RESISTANCE_LEVEL_INDEX] = ergo->GetResistanceLevel();
    // First 4 bits are reserved for future use
    buf[DATA_PAGE_17_CAPABILITIES_FE_STATE_INDEX] = ergo->GetCapabilitiesState() & 0xF0;
}

void AntFECProfile::DataPage25(uint8_t *buf) {
    uint16_t power = ergo->GetPower();
    uint8_t trainerStatus = ergo->GetTrainerStatusBitField();
    uint8_t trainerTargetPowerFlag = ergo->GetTargetPowerFlag();
    uint8_t trainerFEBits = ergo->GetFEStateBits();
    buf[DATA_PAGE_25_DATA_PAGE_NUMBER_INDEX] = 25;
    buf[DATA_PAGE_25_UPDATE_EVENT_COUNT_INDEX] = ++eventCounter;
    buf[DATA_PAGE_25_INSTANTANEOUS_CADENCE_INDEX] = ergo->GetCadence();
    buf[DATA_PAGE_25_ACCUMULATED_POWER_LSB_INDEX] = (uint8_t) accumulatedPower;
    buf[DATA_PAGE_25_ACCUMULATED_POWER_MSB_INDEX] = (uint8_t) (accumulatedPower >> 8);
    buf[DATA_PAGE_25_INSTANTANEOUS_POWER_LSB_INDEX] = power;
    buf[DATA_PAGE_25_INSTANTANEOUS_POWER_MSB_BIT_STATUS_INDEX] = (power >> 8) + (trainerStatus << 4);
    buf[DATA_PAGE_25_FLAG_FE_STATE_INDEX] = trainerTargetPowerFlag + (trainerFEBits << 4);
}

void AntFECProfile::DataPage26(uint8_t *buf) {
}

void AntFECProfile::DataPage48(uint8_t *buf) {
}

void AntFECProfile::DataPage49(uint8_t *buf) {
}

void AntFECProfile::DataPage50(uint8_t *buf) {
}

void AntFECProfile::DataPage51(uint8_t *buf) {
}

void AntFECProfile::DataPage54(uint8_t *buf) {
}

void AntFECProfile::DataPage55(uint8_t *buf) {
}

void AntFECProfile::DataPage71(uint8_t *buf) {
}
void AntFECProfile::DataPage80(uint8_t *buf) {
}
void AntFECProfile::DataPage81(uint8_t *buf) {
}


