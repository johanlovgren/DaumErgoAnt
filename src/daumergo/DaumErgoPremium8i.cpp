//
// Created by Johan Lövgren on 2021-08-12.
//
#include "DaumErgoPremium8i.h"
#include <thread>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <cmath>
#include <vector>

// Port according to Daum interface documentation
#define PORT 51955
#define SOCKET_TIMEOUT 3

#define ASCII_NUMBER_OFFSET '0'
#define MAX_SEND_TRIES 3

#define TO_KMH 3.6
#define TO_MPS 1/3.6

// Buffer elements according to Daum communication documentation
#define HEADER_INDEX0 0
#define HEADER_INDEX1 1
#define HEADER_INDEX2 2
#define SOH_INDEX 0
#define S1 1
#define Z1 2
#define Z2 3
#define FIRST_DATA_INDEX 4


// Message buffer characters
#define SOH 0x01
#define ETB 0x17
#define GS 0x1d
#define ACK 0x06
#define NAK 0x15

#define ONE_BYTE 1

// Message headers
#define TRAINING_DATA_COMPLETE "X70"
#define DEVICE_TYPE "Y00"

// ----------------------- Public -------------------------------


DaumErgoPremium8i::DaumErgoPremium8i(bool verbose) {
    verbose = verbose;
    sock = 0;
    valRead = 0;

    time = 0; heartRate = 0; distance = 0;
    currentSpeed = 0; inclination = 0; physicalEnergy = 0;
    realisticEnergy = 0; torque = 0; gear = 0;
    onOff = 0; rotationalSpeedStatus = 0;

    memset(sendBuffer, 0, ERGOP8I_MAX_BUFFER_SIZE);
    memset(receiveBuffer, 0, ERGOP8I_MAX_BUFFER_SIZE);
}

bool DaumErgoPremium8i::Init(const char *ip) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "Could not create socket" << std::endl;
        return false;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    if ((inet_pton(AF_INET, ip, &serverAddress.sin_addr)) <= 0) {
        std::cerr << "Could not convert IP address" << std::endl;
        return false;
    }

    int connection = (connect(sock, (struct sockaddr*) &serverAddress, sizeof(serverAddress)));
    if (connection < 0) {
        std::cerr << "Could not connect IP address: " << connection << "\nErrno: " << errno << std::endl;
        return false;
    }
    if (verbose)
        std::cout << "Daum Ergo Premium 8i connection initialized" << std::endl;

    initialized = true;
    return true;
}

void DaumErgoPremium8i::DataUpdater() {
    while (!done) {
        UpdateTrainingDataComplete();
        std::this_thread::sleep_for(std::chrono::milliseconds (500));
    }
}

bool DaumErgoPremium8i::RunDataUpdater() {
    if (!initialized) {
        std::cerr << "Not initialized" << std::endl;
        return false;
    }
    done = false;
    std::thread (&DaumErgoPremium8i::DataUpdater, this).detach();
    return true;
}

bool DaumErgoPremium8i::RunWorkout(std::vector<std::tuple<int, int>>) {
    std::cerr << "Not implemented" << std::endl;
    return false;
}

void DaumErgoPremium8i::Close() {
    dataMutex.lock();
    done = true;
    close(sock);
    dataMutex.unlock();
}

// TODO Implement the following methods
uint8_t DaumErgoPremium8i::GetEquipmentType() {
    return 0;
}

uint16_t DaumErgoPremium8i::GetElapsedTime() {
    return 0;
}

uint16_t DaumErgoPremium8i::GetDistanceTraveled() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetHeartRate() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetCapabilitiesState() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetCycleLength() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetResistanceLevel() {
    return 0;
}

uint16_t DaumErgoPremium8i::GetIncline() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetFEStateBits() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetTargetPowerFlag() {
    return 0;
}

uint8_t DaumErgoPremium8i::GetTrainerStatusBitField() {
    return 0;
}

// ----------------------- Private -------------------------------

/**
 * Calculates the checksum for the message buffer according to the Daum documentation
 * @param messageBuffer to be sent to the ergo
 * @return the checksum for messageBuffer
 */
u_short Checksum(const char messageBuffer[]) {
    int i = 1;
    u_short checksum = 0;
    while(messageBuffer[i] != ETB && messageBuffer[i]) {
        checksum += messageBuffer[i++];
    }
    return checksum % 100;
}

/**
 * Validates the checksum for a received message buffer
 * @param messageBuffer the received message buffer
 * @return true if valid, otherwise false
 */
bool CheckChecksum(const char messageBuffer[]) {
    u_short checksum = Checksum(messageBuffer);
    int len = strlen((char*) messageBuffer);
    return checksum / 10 == messageBuffer[len-3] &&
           checksum % 10 == messageBuffer[len-2];
}

/**
 * Print a message buffer
 * @param messageBuffer to be printed
 */
void PrintMessageBuffer(char messageBuffer[]){
    for (int i = 0; messageBuffer[i] != ETB || i >= ERGOP8I_MAX_BUFFER_SIZE; ++i) {
        std::cout << messageBuffer[i] << std::endl;
    }
}

/**
 * Fetches the training data from the ergo and updates the class variables
 */
void DaumErgoPremium8i::UpdateTrainingDataComplete() {
    this->Query(TRAINING_DATA_COMPLETE);
    dataMutex.lock();
    time = std::stoi(strtok((char*) receiveBuffer+FIRST_DATA_INDEX, (const char*) "\x1d"));
    heartRate = std::stoi(strtok(nullptr, (const char*) "\x1d"));
    currentSpeed = round(std::stoi(strtok(nullptr, (const char*) "\x1d")) * TO_KMH);
    inclination = std::stof(strtok(nullptr, (const char*) "\x1d"));
    distance = std::stoi(strtok(nullptr, (const char*) "\x1d"));
    currentCadence = round(std::stof(strtok(nullptr, (const char*) "\x1d")));
    currentPower = std::stoi(strtok(nullptr, (const char*) "\x1d"));
    physicalEnergy = std::stof(strtok(nullptr, (const char*) "\x1d"));
    realisticEnergy = std::stof(strtok(nullptr, (const char*) "\x1d"));
    torque = std::stof(strtok(nullptr, (const char*) "\x1d"));
    gear = strtok(nullptr, (const char *) "\x1d")[0]; // Gear
    onOff = strtok(nullptr, (const char*) "\x1d")[0]; // OnOff
    rotationalSpeedStatus = strtok(nullptr, (const char*) "\x1d")[0];
    dataMutex.unlock();

}

/**
 * Makes a query to the ergo bike
 * @param header for the query, tells what is asked from the ergo
 */
void DaumErgoPremium8i::Query(const char *header){
    memset(sendBuffer, 0, ERGOP8I_MAX_BUFFER_SIZE);
    sendBuffer[SOH_INDEX] = SOH;
    sendBuffer[S1] = header[HEADER_INDEX0];
    sendBuffer[Z1] = header[HEADER_INDEX1];
    sendBuffer[Z2] = header[HEADER_INDEX2];
    u_short checksum = Checksum(sendBuffer);
    sendBuffer[FIRST_DATA_INDEX] = (checksum / 10) + ASCII_NUMBER_OFFSET;
    sendBuffer[FIRST_DATA_INDEX + 1] = (checksum % 10) + ASCII_NUMBER_OFFSET;
    sendBuffer[FIRST_DATA_INDEX + 2] = ETB;

    if (!SendReceiveMessage())
        std::cerr << "Failed query" << std::endl;
}

/**
 * Sends a query and waits for an answer from the ergo, sends ACK on successful answer.
 * @return tru if successful, otherwise false
 */
bool DaumErgoPremium8i::SendReceiveMessage() {
    int tries = 0;
    valRead = 0;
    while (tries < MAX_SEND_TRIES) {
        send(sock, sendBuffer, strlen((const char *) (sendBuffer)), 0); // Check for error?
        valRead = read(sock, &receiveBuffer, ERGOP8I_MAX_BUFFER_SIZE);
        if (receiveBuffer[0] == ACK) {
            break;
        }
        tries++;
    }
    if (tries >= MAX_SEND_TRIES)
        return false;
    valRead = read(sock, &receiveBuffer, ERGOP8I_MAX_BUFFER_SIZE);
    sendBuffer[HEADER_INDEX0] = ACK;
    send(sock, sendBuffer, ONE_BYTE, 0);
    return true;
}


