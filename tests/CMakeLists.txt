project(Google_tests)

include_directories(../googletest/googletest/include ../googletest/googletest)

set(TEST_FILES
        ant/AntBicyclePowerTests.cpp)

add_executable(Google_Tests_run
        ${TEST_FILES})
target_link_libraries(Google_Tests_run
        gtest
        gtest_main)