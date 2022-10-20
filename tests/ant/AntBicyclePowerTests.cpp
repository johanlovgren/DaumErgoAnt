//
// Created by Johan Lövgren on 2022-10-20.
//
#include "gtest/gtest.h"

TEST (ANTTest, PosZeroNeg) {
EXPECT_EQ (9.0, (3.0*3.0));
ASSERT_EQ (0.0, (0.0));
ASSERT_EQ (9, (-3)*(-3.0));
}