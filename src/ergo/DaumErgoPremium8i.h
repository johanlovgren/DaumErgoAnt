//
// Created by Johan Lövgren on 2021-08-12.
//

#ifndef DAUMERGOANT_DAUMERGOPREMIUM8I_H
#define DAUMERGOANT_DAUMERGOPREMIUM8I_H

#include "DaumErgo.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


#define ARG_P8I "p8i"
#define ERGOP8I_MAX_BUFFER_SIZE 1024
/**
 * DaumErgo class for the Premium 8i bike.
 * Currently only supporting the bike, but other devices may be supported with minor changes
 */
class DaumErgoPremium8i: public DaumErgo {
public:
    /**
     * Default constructor
     * @param verbose output enabled if true
     */
    explicit DaumErgoPremium8i(bool verbose);
    /**
     * Initializes the DaumErgoPremium8i, setting up the TCP connection
     * @param ip address of the ergo which to connect
     * @return true if successful, otherwise false
     */
    bool Init(const char* ip) override;
    /**
     * Closes the DaumErgoPremium8i, closing the TCP connection and stops the data updater
     */
    void Close() override;
    /**
     * Starts the data updater which updates the data periodically
     * @return true if started successfully, otherwise false
     */
    bool RunDataUpdater() override;

    /**
     * Starts a workout on the bike.
     * @param workout The workout to run, where each tuple in the vector defines how long a certain wattage should hold
     * @return True if workout started, otherwise false
     */
    bool RunWorkout(std::vector<std::tuple<int, int>> workout) override;

private:
    void UpdateTrainingDataComplete();
    void DataUpdater();
    bool SendReceiveMessage();
    void Query(const char *header);

    int sock, valRead;
    struct sockaddr_in serverAddress{};
    char sendBuffer[ERGOP8I_MAX_BUFFER_SIZE]{};
    char receiveBuffer[ERGOP8I_MAX_BUFFER_SIZE]{};

    u_int time, heartRate, distance;
    float inclination, physicalEnergy, realisticEnergy, torque;
    char gear, onOff, rotationalSpeedStatus;
};


#endif //DAUMERGOANT_DAUMERGOPREMIUM8I_H
