//
// Created by Johan Lövgren on 2022-10-20.
//

#ifndef DAUMERGOANT_TESTERGO_H
#define DAUMERGOANT_TESTERGO_H

#include "DaumErgo.h"

class TestErgo: public DaumErgo {
public:
    explicit TestErgo(bool verbose);
    ~TestErgo();
    void SetPower(unsigned short power);
    void SetCadence(unsigned short cadence);
    void SetSpeed(unsigned short speed);

    bool Init(const char *string) override;

    void Close() override;

    bool RunDataUpdater() override;

    bool RunWorkout(std::vector<std::tuple<int, int>> workout) override;

    uint8_t GetEquipmentType() override;

    uint16_t GetElapsedTime() override;

    uint16_t GetDistanceTraveled() override;

    uint8_t GetHeartRate() override;

    uint8_t GetCapabilitiesState() override;

    uint8_t GetCycleLength() override;

    uint8_t GetResistanceLevel() override;

    uint16_t GetIncline() override;

    uint8_t GetFEStateBits() override;

    uint8_t GetTargetPowerFlag() override;

    uint8_t GetTrainerStatusBitField() override;
};


#endif //DAUMERGOANT_TESTERGO_H
