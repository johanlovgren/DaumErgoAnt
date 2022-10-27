//
// Created by Johan Lövgren on 2022-10-25.
//

//
// Created by Johan Lövgren on 2022-10-20.
//
#include "gtest/gtest.h"
#include "../../src/daumergo/DaumErgo8008TRS.h"
#include "../../src/daumergo/TestErgo.h"
#include "../../include/antdefines.h"

#define BYTE_MAX 256
#define TWO_BYTE_MAX 65536

TEST (Daum8008TRS, Constants) {
    auto *ergo = new DaumErgo8008TRS(false);

    ASSERT_EQ(ergo->GetEquipmentType(), 25);
    ASSERT_EQ(ergo->GetHrDistanceSpeedCapabilitiesBits(), 0);
    ASSERT_EQ(ergo->GetTrainerPowerStatusBitField(), 0);
}



