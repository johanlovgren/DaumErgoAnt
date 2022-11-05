//
// Created by Johan Lövgren on 2022-03-30.
//

#include "AntFECProfile.h"
#include <iostream>

/******************************************** Constants ********************************************/
#define MASTER 0x10
#define RF_CHANNEL_FREQUENCY 57
#define DEVICE_TYPE 0x11
#define TRANS_TYPE 5
#define DEVICE_NUMBER 51
#define CHANNEL_PERIOD 8192

// Transmit pattern constants
#define TRANSMIT_PATTERN_BLOCK_1 64
#define TRANSMIT_PATTERN_BLOCK_2 66
#define TRANSMIT_PATTERN_BLOCK_3 130
#define TRANSMIT_PATTERN_BLOCK_4 132
#define TRANSMIT_PATTERN_B_BLOCK_1 2
#define TRANSMIT_PATTERN_B_BLOCK_2 4

#define DP_NUMBER_INDEX 0

// Data Page 16 - General FE Data
#define DP16_EQUIPMENT_TYPE_INDEX 1
#define DP16_ELAPSED_TIME_INDEX 2 // Accumulated with rollover every 64 second
#define DP16_DISTANCE_TRAVELED_INDEX 3 // Accumulated with rollover every 256 meter
#define DP16_SPEED_LSB_INDEX 4 // Instantaneous speed sent in units of 0.001 m/s
#define DP16_SPEED_MSB_INDEX 5
#define DP16_HEART_RATE_INDEX 6 // May be set to invalid, 0xFF
#define DP16_CAPABILITIES_FE_STATE_INDEX 7

#define DP16_ELAPSED_TIME_ROLLOVER 64
#define DP16_DISTANCE_TRAVELED_ROLLOVER 256


// Data Page 17 - General Settings Page
#define DP17_CYCLE_LENGTH_INDEX 3 // Length of one "cycle", trainer = wheel circumference
#define DP17_INCLINE_LSB_INDEX 4 // Percentage between -100% and +100%, 0xFF for invalid
#define DP17_INCLINE_MSB_INDEX 5
#define DP17_RESISTANCE_LEVEL_INDEX 6 // Percentage of max resistance of equipment 0-100% with a 0.5% unit
#define DP17_CAPABILITIES_FE_STATE_INDEX 7

// Data Page 25 - Specific Trainer/Stationary Bike Data
#define DP25_UPDATE_EVENT_COUNT_INDEX 1 // Event counter increments with each information update
#define DP25_INSTANTANEOUS_CADENCE_INDEX 2 // Crank cadence, 0-254 RPM
#define DP25_ACCUMULATED_POWER_LSB_INDEX 3 // Accumulated power, 1 watt-resolution, 65536W rollover
#define DP25_ACCUMULATED_POWER_MSB_INDEX 4
#define DP25_INSTANTANEOUS_POWER_LSB_INDEX 5 // Instantaneous power, 0-4094W
#define DP25_INSTANTANEOUS_POWER_MSB_BIT_STATUS_INDEX 6 // Bits 0-3 instantaneous power MSB, bits 4-7 trainer status
#define DP25_FLAG_FE_STATE_INDEX 7 // Bits 0-3 flag bits, bits 4-7 FE State bits


// Data Page 48 - Basic Resistance
#define DP48_TOTAL_RESISTANCE_INDEX 7

// Data Page 49 - Target Power
#define DP49_TARGET_POWER_LSB_INDEX 6 // The target power for a controllable trainer operating in target power mode
#define DP49_TARGET_POWER_MSB_INDEX 7

// Data Page 50 - Wind Resistance
#define DP50_WIND_RESISTANCE_COEFFICIENT_INDEX 5 // Product of Frontal Surface Area, Drag and Air density
#define DP50_WIND_SPEED_INDEX 6 // Speed of simulated wind acting on the cyclist
#define DP50_WIND_DRAFTING_FACTOR_INDEX 7

// Data Package 51 - Track Resistance
#define DP51_GRADE_LSB_INDEX 5 // Grade of simulated track
#define DP51_GRADE_MSB_INDEX 6
#define DP51_ROLLING_RESISTANCE_COEFFICIENT_INDEX 7 // Coefficient of rolling resistance between bicycle tire and track

// Data Page 54 - FE Capabilities
#define DP54_MAXIMUM_RESISTANCE_LSB_INDEX 5 // Max applicable resistance of the trainer
#define DP54_MAXIMUM_RESISTANCE_MSB_INDEX 6
#define DP54_CAPABILITIES_INDEX 5 // Refer to table 8-46 in documentation

// Data Page 55 - User Configuration
#define DP55_BICYCLE_WHEEL_DIAMETER_OFFSET_WEIGHT_INDEX 4 // Bit 0-3 Bike wheel diameter offset, 4-7 weight LSN
#define DP55_BICYCLE_WEIGHT_MSB_INDEX 5 // MSB of bicycle weight entered on the display (0xFF Invalid)
#define DP55_BICYCLE_WHEEL_DIAMETER_INDEX 6 // Bicycle wheel diameter entered on the display (0xFF Invalid)
#define DP55_GEAR_RATIO_INDEX 7 // Front:Back Gear Ratio entered on the display (0x00 Invalid)

// Required Common Pages
// Common Page 70 - Request Data Page
#define DP70_REQUESTED_RESPONSES_INDEX 5
#define DP70_REQUESTED_PAGE_INDEX 6

// Common Page 71 - Command Status
#define DP71_LAST_RECEIVED_COMMAND_ID_INDEX 1 // Indicates data page number of the last control page received
#define DP71_SEQUENCE_NUMBER_INDEX 2 // Sequence number used by slave in last received command sequence
#define DP71_COMMAND_STATUS_INDEX 3
#define DP71_DATA_INDEX 4 // First data index

// Common Page 80 - Manufacturer's Identification
#define DP80_RESERVED_INDEX1 1
#define DP80_RESERVED_INDEX2 2
#define DP80_HW_REVISION_INDEX 3
#define DP80_MANUFACTURER_ID_LSB_INDEX 4
#define DP80_MANUFACTURER_ID_MSB_INDEX 5
#define DP80_MODEL_NUMBER_LSB_INDEX 6
#define DP80_MODEL_NUMBER_MSB_INDEX 7

// Common Page 81 - Product Information
#define DP81_RESERVED_INDEX 1
#define DP81_SW_REVISION_INDEX 2
#define DP81_SW_REVISION_MAIN_INDEX 3
#define DP81_SERIAL_NUMBER_0_7_INDEX 4
#define DP81_SERIAL_NUMBER_8_15_INDEX 5
#define DP81_SERIAL_NUMBER_16_23_INDEX 6
#define DP81_SERIAL_NUMBER_24_31_INDEX 7

#define NO_RECEIVED_COMMAND 0xFF

/******************************************** Internal headers ********************************************/
void DataPage16(uint8_t *txBuf, DaumErgo *ergo);

void DataPage25(uint8_t *txBuf, DaumErgo *ergo);

void DataPage54(uint8_t *txBuf, DaumErgo *ergo);

void DataPage55(uint8_t *txBuf);

void DataPage71(uint8_t *txBuf,
                uint8_t latestReceivedCommandID,
                uint8_t latestSlaveSequenceNumber,
                uint8_t latestCommandStatus,
                DaumErgo *ergo);

void DataPage80(uint8_t *txBuf);

void DataPage81(uint8_t *txBuf);

bool handleDataPage70Request(uint8_t requestedPage,
                             uint8_t *txBuf,
                             uint8_t latestReceivedCommandID,
                             uint8_t latestSlaveSequenceNumber,
                             uint8_t latestCommandStatus,
                             DaumErgo *ergo);

bool handleDataPage48Request(uint8_t *rxBuf, uint8_t *txBuf, DaumErgo *ergo);

bool handleDataPage49Request(uint8_t *rxBuf, uint8_t *txBuf, DaumErgo *ergo);

bool handleDataPage55Request();

void printTransmissionBuffer2(UCHAR *aucTransmitBuffer, uint8_t len);


/******************************************** Public ********************************************/

AntFECProfile::AntFECProfile(DaumErgo *ergo) {
    this->ergo = ergo;
    this->deviceType = DEVICE_TYPE;
    this->channelPeriod = CHANNEL_PERIOD;
    this->transType = TRANS_TYPE;
    this->channelType = PARAMETER_TX_NOT_RX;
    this->deviceNum = DEVICE_NUMBER;
    this->latestReceivedCommandID = NO_RECEIVED_COMMAND;
    this->latestSlaveSequenceNumber = NO_RECEIVED_COMMAND;
    this->latestCommandStatus = NO_RECEIVED_COMMAND;
    transmitPatternCounter = transmitPatternBCounter = 0;
}

AntFECProfile::~AntFECProfile() {
    this->ergo = nullptr;
}

/**
 * Handles an ANT+ TX Event
 * @param txBuffer Data that will be sent as broadcast data. Buffer will be filled by this method.
 */
void AntFECProfile::HandleTXEvent(uint8_t *txBuffer) {
    TransmitPatternB(txBuffer);
}

/**
 * Handles a Acknowledged Event that was received from slave.
 * @param rxBuf Data received from slave
 * @param txBuf Data that will be returned to slave. Will be filled by method.
 * @return How many times the response is requested to be returned to slave if request was handled successful. If no
 * response shall be sent, 0xFF will be returned.
 */
uint8_t AntFECProfile::HandleAckEvent(uint8_t *rxBuf, uint8_t *txBuf) {
    printTransmissionBuffer2(rxBuf, 8);
    uint8_t rxDataPageNumber = rxBuf[DP_NUMBER_INDEX];
    bool supported;

    switch (rxDataPageNumber) {
        case 70:
            supported = handleDataPage70Request(rxBuf[DP70_REQUESTED_PAGE_INDEX],
                                                txBuf,
                                                latestReceivedCommandID,
                                                latestSlaveSequenceNumber,
                                                latestCommandStatus,
                                                ergo);
            break;
        case 48:
            supported = handleDataPage48Request(rxBuf, txBuf, ergo);
            break;
        case 49:
            supported = handleDataPage49Request(rxBuf, txBuf, ergo);
            break;
        case 55:
            supported = handleDataPage55Request();
            break;
        default:
            supported = false;
    }

    if (!supported)
        return NOT_SUPPORTED;

    if (rxDataPageNumber == 70)
        return rxBuf[DP70_REQUESTED_RESPONSES_INDEX];

    latestReceivedCommandID = rxDataPageNumber;
    ++latestSlaveSequenceNumber;
    latestCommandStatus = 0;
    return 1;
}


/******************************************** Private ********************************************/

/**
 * Prints a buffer. Used for debugging.
 * @param aucTransmitBuffer Buffer to print from
 * @param len How many bytes to print
 */
void printTransmissionBuffer2(UCHAR *aucTransmitBuffer, uint8_t len) {
    for (int i = 0; i < len; ++i) {
        printf("[%02x] ", aucTransmitBuffer[i]);
    }
    std::cout << ": ";
    for (int i = 0; i < len; ++i) {
        printf("[%d] ", aucTransmitBuffer[i]);
    }
    std::cout << std::endl;
}


/**
 * Handles a Request Data Page (Data Page 70) request from slave
 * Read more about Data Pate 70 on page 14 in ANT+ Common Data Page documentation.
 * @param requestedPage What page that was requested from slave
 * @param txBuf Buffer to fill data that is to be returned in
 * @param latestReceivedCommandID Latest successfully received command page
 * @param latestSlaveSequenceNumber Sequence number of latest successfully received command page
 * @param latestCommandStatus Status of handling the latest received command page
 * @param ergo Ergo to gather data from
 * @return True if the request is successfully filled in txBuf, otherwise False. If False, request from slave should
 * be ignored
 */
bool handleDataPage70Request(uint8_t requestedPage,
                             uint8_t *txBuf,
                             uint8_t latestReceivedCommandID,
                             uint8_t latestSlaveSequenceNumber,
                             uint8_t latestCommandStatus,
                             DaumErgo *ergo) {
    switch (requestedPage) {
        case 80:
            DataPage80(txBuf);
            return true;
        case 54:
            DataPage54(txBuf, ergo);
            return true;
        case 55:
            DataPage55(txBuf);
            return false; // TODO change when implemented
        case 71:
            DataPage71(txBuf, latestReceivedCommandID, latestSlaveSequenceNumber, latestCommandStatus, ergo);
            return true;
        default:
            return false;
    }
}

/**
 * Handles a Basic Resistance (Data Page 48) request from slave.
 * Read more about Data Page 48 on page 53 in ANT+ FEC documentation.
 * @param rxBuf Data received from slave.
 * @param txBuf Buffer that will be returned to Slave. Filled by this function.
 * @param ergo Ergo to gather data from.
 * @return True if the request is successfully filled in txBuf, otherwise False. If False, request from slave should
 * be ignored
 */
bool handleDataPage48Request(uint8_t *rxBuf, uint8_t *txBuf, DaumErgo *ergo) {
    // Resistance level is in 0.5% unit
    uint8_t targetResistance = rxBuf[DP48_TOTAL_RESISTANCE_INDEX] / 2;
    ergo->SetResistance(targetResistance);
    std::copy(rxBuf, rxBuf + ANT_STANDARD_DATA_PAYLOAD_SIZE, txBuf);
    return true;
}

/**
 * Handles a Target Power (Data Page 49) request from slave.
 * Read more about Data Page 49 on page 54 in ANT+ FEC documentation
 * @param rxBuf Data received from slave
 * @param txBuf Buffer that will be returned to Slave. Filled by this function.
 * @param ergo Ergo to gather data from.
 * @return True if the request is successfully filled in txBuf, otherwise False. If False, request from slave should
 * be ignored
 */
bool handleDataPage49Request(uint8_t *rxBuf, uint8_t *txBuf, DaumErgo *ergo) {
    // Power unit in rxBuf is 0.25W
    uint16_t targetPower = (rxBuf[DP49_TARGET_POWER_LSB_INDEX] + (rxBuf[DP49_TARGET_POWER_MSB_INDEX] << 8)) / 4;
    ergo->SetPower(targetPower);
    std::copy(rxBuf, rxBuf + ANT_STANDARD_DATA_PAYLOAD_SIZE, txBuf);
    return true;
}

/**
 * Handles a User Configuration (Data Page 55) request from slave.
 * Read more about Data Page 55 on page 61 in ANT+ FEC documentation
 * @return False due to no implementation!
 */
bool handleDataPage55Request() {
    // TODO Implement this!
    return false;
}

/**
 * Used to transmit broadcast messages via transmit pattern a) shown in ANT+ FEC documentation on page 24.
 * Data that will be transmitted is Data Page 16, 80 and 81.
 * @param txBuf Data that will be sent as broadcast data. Will be filled by this method.
 */
void AntFECProfile::TransmitPatternA(uint8_t *txBuf) {
    ++transmitPatternCounter;
    if (transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_1 ||
        (transmitPatternCounter >= TRANSMIT_PATTERN_BLOCK_2 &&
         transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_3)) {
        DataPage16(txBuf, ergo);
    } else if (transmitPatternCounter >= TRANSMIT_PATTERN_BLOCK_1 &&
               transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_2) {
        DataPage80(txBuf);
    } else if (transmitPatternCounter >= TRANSMIT_PATTERN_BLOCK_3 &&
               transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_4) {
        DataPage81(txBuf);
    } else {
        transmitPatternCounter = 0;
        DataPage16(txBuf, ergo);
    }
}

/**
 * Used to transmit broadcast messages via transmit pattern b) shown in ANT+ FEC documentation on page 24.
 * Data that will be transmitted is Data Page 16, 25, 80 and 81.
 * @param txBuf Data that will be sent as broadcast data. Will be filled by this method.
 */
void AntFECProfile::TransmitPatternB(uint8_t *txBuf) {
    ++transmitPatternCounter;
    if (transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_1 ||
        (transmitPatternCounter >= TRANSMIT_PATTERN_BLOCK_2 &&
         transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_3)) {
        ++transmitPatternBCounter;
        if (transmitPatternBCounter >= 0 &&
            transmitPatternBCounter < TRANSMIT_PATTERN_B_BLOCK_1) {
            DataPage16(txBuf, ergo);
        } else if (transmitPatternBCounter >= TRANSMIT_PATTERN_B_BLOCK_1 &&
                   transmitPatternBCounter < TRANSMIT_PATTERN_B_BLOCK_2) {
            DataPage25(txBuf, ergo);
        } else {
            transmitPatternBCounter = 0;
            DataPage16(txBuf, ergo);
        }
    } else if (transmitPatternCounter >= TRANSMIT_PATTERN_BLOCK_1 &&
               transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_2) {
        DataPage80(txBuf);
    } else if (transmitPatternCounter >= TRANSMIT_PATTERN_BLOCK_3 &&
               transmitPatternCounter < TRANSMIT_PATTERN_BLOCK_4) {
        DataPage81(txBuf);
    } else {
        transmitPatternCounter = 0;
        DataPage16(txBuf, ergo);
    }
}

/**
 * Fills a transmit buffer with General FE Data (Data Page 16).
 * Read more about Data Page 16 on page 29 in ANT+ FEC documentation.
 * @param txBuf Data that shall be sent to slave. Buffer will be filled by this function.
 * @param ergo Ergo to gather data from.
 */
void DataPage16(uint8_t *txBuf, DaumErgo *ergo) {
    // Converting speed to unit of 0.001 m/s
    uint16_t speed = (ergo->GetSpeed() / 3.6) * 1000;
    uint8_t capabilitiesBits = ergo->GetHrDistanceSpeedCapabilitiesBits();
    uint8_t feState = ergo->GetFEStateBits();

    txBuf[DP_NUMBER_INDEX] = 16;
    txBuf[DP16_EQUIPMENT_TYPE_INDEX] = ergo->GetEquipmentType();
    // Elapsed time have 0.25s as unit and is an accumulated value
    txBuf[DP16_ELAPSED_TIME_INDEX] = (ergo->GetElapsedTime() * 4) % DP16_ELAPSED_TIME_ROLLOVER;
    // Distance traveled have meter unit and is an accumulated value
    txBuf[DP16_DISTANCE_TRAVELED_INDEX] = ergo->GetDistanceTraveled() % DP16_DISTANCE_TRAVELED_ROLLOVER;
    txBuf[DP16_SPEED_LSB_INDEX] = (uint8_t) speed;
    txBuf[DP16_SPEED_MSB_INDEX] = (uint8_t) (speed >> 8);
    txBuf[DP16_HEART_RATE_INDEX] = ergo->GetHeartRate();
    txBuf[DP16_CAPABILITIES_FE_STATE_INDEX] = capabilitiesBits + (feState << 4);
}

/**
 * Fills a transmit buffer with General Settings data (Data Page 17).
 * Read more about Data Page 17 on page 33 in ANT+ FEC documentation.
 * @param txBuf Data that shall be sent to slave. Buffer will be filled by this function.
 * @param ergo Ergo to gather data from.
 */
void DataPage17(uint8_t *txBuf, DaumErgo *ergo) {
    // TODO Needs to be properly implemented
    uint16_t incline = ergo->GetIncline();
    txBuf[DP_NUMBER_INDEX] = 17;
    txBuf[DP17_CYCLE_LENGTH_INDEX] = ergo->GetCycleLength();
    txBuf[DP17_INCLINE_LSB_INDEX] = (uint8_t) incline;
    txBuf[DP17_INCLINE_MSB_INDEX] = (uint8_t) (incline >> 8);
    txBuf[DP17_RESISTANCE_LEVEL_INDEX] = ergo->GetResistanceLevel();
    // First 4 bits are reserved for future use
    txBuf[DP17_CAPABILITIES_FE_STATE_INDEX] = ergo->GetHrDistanceSpeedCapabilitiesBits() & 0xF0;
}

/**
 * Fills a transmit buffer with Specific Trainer/Stationary Bike Data (Data Page 25).
 * Read more about Data Page 25 on page 45 in ANT+ FEC documentation.
 * @param txBuf Data that shall be sent to slave. Buffer will be filled by this function.
 * @param ergo Ergo to gather data from.
 */
void DataPage25(uint8_t *txBuf, DaumErgo *ergo) {
    uint16_t power = ergo->GetPower();
    uint16_t accumulatedPower = ergo->GetAccumulatedPower();
    uint8_t trainerStatus = ergo->GetTrainerPowerStatusBitField();
    uint8_t trainerTargetPowerFlag = ergo->GetTargetPowerFlag();
    uint8_t trainerFEBits = ergo->GetFEStateBits();

    txBuf[DP_NUMBER_INDEX] = 25;
    txBuf[DP25_UPDATE_EVENT_COUNT_INDEX] = ergo->GetPowerEventCounter();
    txBuf[DP25_INSTANTANEOUS_CADENCE_INDEX] = ergo->GetCadence();
    txBuf[DP25_ACCUMULATED_POWER_LSB_INDEX] = (uint8_t) accumulatedPower;
    txBuf[DP25_ACCUMULATED_POWER_MSB_INDEX] = (uint8_t) (accumulatedPower >> 8);
    txBuf[DP25_INSTANTANEOUS_POWER_LSB_INDEX] = power;
    txBuf[DP25_INSTANTANEOUS_POWER_MSB_BIT_STATUS_INDEX] = (power >> 8) + (trainerStatus << 4);
    txBuf[DP25_FLAG_FE_STATE_INDEX] = trainerTargetPowerFlag + (trainerFEBits << 4);
}

/**
 * Fills a transmission buffer with FE Capabilities (Data Page 54). This page shall be sent on request from slave.
 * Read more about Data Page 54 on page 60 in ANT+ FEC documentation.
 * @param txBuf Data that shall be sent to slave. Buffer will be filled by this funciton.
 * @param ergo Ergo to gather data from
 */
void DataPage54(uint8_t *txBuf, DaumErgo *ergo) {
    // Fills unused indexes
    memset(txBuf, 0xFF, 4);
    txBuf[DP_NUMBER_INDEX] = 54;
    txBuf[DP54_MAXIMUM_RESISTANCE_LSB_INDEX] = 0xFF;
    txBuf[DP54_MAXIMUM_RESISTANCE_MSB_INDEX] = 0xFF; // 0xFFFF for invalid
    txBuf[DP54_CAPABILITIES_INDEX] = ergo->GetTrainingModeCapabilities();

}

/**
 * Fills a transmission buffer with User Configuration data (Data Page 55). This page shall be sent on request from
 * slave.
 * Read more about Data Page 55 on page 61 in ANT+ FEC documentation.
 * @param txBuf
 */
void DataPage55(uint8_t *txBuf) {
    // TODO Implement this!
}

/**
 * Fills a transmission buffer with Command Status data (Data Page 71). This page shall be sent on request from slave.
 * Read more about Data Page 71 on page 63 in ANT+ FEC documentation
 * @param txBuf
 */
void DataPage71(uint8_t *txBuf,
                uint8_t latestReceivedCommandID,
                uint8_t latestSlaveSequenceNumber,
                uint8_t latestCommandStatus,
                DaumErgo *ergo) {
    txBuf[DP_NUMBER_INDEX] = 71;
    txBuf[DP71_LAST_RECEIVED_COMMAND_ID_INDEX] = latestReceivedCommandID;
    txBuf[DP71_SEQUENCE_NUMBER_INDEX] = latestSlaveSequenceNumber;
    txBuf[DP71_COMMAND_STATUS_INDEX] = latestCommandStatus;
    if (latestReceivedCommandID == 48) {
        txBuf[DP71_DATA_INDEX] = 0xFF;
        txBuf[DP71_DATA_INDEX + 1] = 0xFF;
        txBuf[DP71_DATA_INDEX + 2] = 0xFF;
        txBuf[DP71_DATA_INDEX + 3] = ergo->GetTargetResistance(); // Total resistance
    } else if (latestReceivedCommandID == 49) {
        // Target Power set to unit of 0.25W
        uint16_t targetPower = ergo->GetTargetPower() * 4;
        txBuf[DP71_DATA_INDEX] = 0xFF;
        txBuf[DP71_DATA_INDEX + 1] = 0xFF;
        txBuf[DP71_DATA_INDEX + 2] = (uint8_t) targetPower; // Target Power LSB
        txBuf[DP71_DATA_INDEX + 3] = (uint8_t) (targetPower >> 8); // Target Power MSB
    } else if (latestReceivedCommandID == 50) {
        txBuf[DP71_DATA_INDEX] = 0xFF;
        txBuf[DP71_DATA_INDEX + 1] = 0xFF; // Wind Resistance Coefficient
        txBuf[DP71_DATA_INDEX + 2] = 0xFF; // Wind Speed
        txBuf[DP71_DATA_INDEX + 3] = 0xFF; // Drafting Factor // TODO Fix when implemented
    } else if (latestReceivedCommandID == 51) {
        txBuf[DP71_DATA_INDEX] = 0xFF;
        txBuf[DP71_DATA_INDEX + 1] = 0xFF; // Grade (Slope) LSB
        txBuf[DP71_DATA_INDEX + 2] = 0xFF; // Grade (Slope) MSB
        txBuf[DP71_DATA_INDEX + 3] = 0xFF; // Rolling resistance coefficient // TODO Fix when implemented
    } else {
        txBuf[DP71_DATA_INDEX] = 0xFF;
        txBuf[DP71_DATA_INDEX + 1] = 0xFF;
        txBuf[DP71_DATA_INDEX + 2] = 0xFF;
        txBuf[DP71_DATA_INDEX + 3] = 0xFF;
    }
}


/**
 * Fills a transmission buffer with Manufacturer's information data (Data Page 80).
 * Read more about Data Page 80 on page 28 in ANT+ Common Data Pages documentation
 * @param txBuf Data that shall be sent to slave. Buffer will be filled by this function.
 */
void DataPage80(uint8_t *txBuf) {
    txBuf[DP_NUMBER_INDEX] = 80;
    txBuf[DP80_RESERVED_INDEX1] = 0xFF;
    txBuf[DP80_RESERVED_INDEX2] = 0xFF;
    txBuf[DP80_HW_REVISION_INDEX] = 0x0A; // 0x0A followed documentation example
    txBuf[DP80_MANUFACTURER_ID_LSB_INDEX] = 0xFF;
    txBuf[DP80_MANUFACTURER_ID_MSB_INDEX] = 0;  // 0x00FF for development ID
    txBuf[DP80_MODEL_NUMBER_LSB_INDEX] = 0xAD;
    txBuf[DP80_MODEL_NUMBER_MSB_INDEX] = 0xDE; // 0xDEAD is just arbitrary model number
}

/**
 * Fills a transmission buffer with Product information data (Data Page 81).
 * Read more about Data Page 81 on page 29 in ANT+ Common Data Pages documentation
 * @param txBuf Data that shall be sent to slave. BUffer will be filled by this funciton.
 */
void DataPage81(uint8_t *txBuf) {
    txBuf[DP_NUMBER_INDEX] = 81;
    txBuf[DP81_RESERVED_INDEX] = 0xFF;
    txBuf[DP81_SW_REVISION_INDEX] = 0xFF;
    txBuf[DP81_SW_REVISION_MAIN_INDEX] = 0x01;
    txBuf[DP81_SERIAL_NUMBER_0_7_INDEX] = 0xFF;
    txBuf[DP81_SERIAL_NUMBER_8_15_INDEX] = 0xFF;
    txBuf[DP81_SERIAL_NUMBER_16_23_INDEX] = 0xFF;
    txBuf[DP81_SERIAL_NUMBER_24_31_INDEX] = 0xFF; // 0xFFFFFFFF for device without serial number
}



