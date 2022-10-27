//
// Created by Johan Lövgren on 2022-10-20.
//

#ifndef DAUMERGOANT_TESTERGO_H
#define DAUMERGOANT_TESTERGO_H

#include "DaumErgo.h"

#define ARG_TEST "test"

class TestErgo: public DaumErgo {
public:
    explicit TestErgo();
    ~TestErgo();
    void SetPower(uint16_t power) override;
    void SetResistance(uint8_t resistance) override;
    void SetCadence(unsigned short cadence);
    void SetSpeed(unsigned short speed);
    void SetEquipmentType(uint8_t equipmentType);


    bool Init(const char *string) override;

    void Close() override;

    bool RunDataUpdater() override;

    bool RunWorkout(std::vector<std::tuple<int, int>> workout) override;

    uint8_t GetCycleLength() override;

    uint8_t GetResistanceLevel() override;

    uint16_t GetIncline() override;
private:
    void DataUpdater();
};


#endif //DAUMERGOANT_TESTERGO_H
