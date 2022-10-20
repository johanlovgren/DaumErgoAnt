project(Google_tests)

include_directories(../googletest/googletest/include ../googletest/googletest)

set(TEST_FILES
        ant/AntBicyclePowerTests.cpp ant/AntBikeSpeedCadenceTests.cpp)

add_executable(Google_Tests_run
        ${TEST_FILES})
target_link_libraries(Google_Tests_run
        ant
        daumergo
        utils
        gtest
        gtest_main)