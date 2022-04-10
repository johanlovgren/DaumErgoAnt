cmake_minimum_required(VERSION 3.17)
project(DaumErgoAnt)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/ant/)
include_directories(src/daumergo)
include_directories(src/libs/includes)


find_library(CORE_FOUNDATION CoreFoundation)
if (NOT CORE_FOUNDATION)
    message(FATAL_ERROR "CoreFoundation not found")
endif()
message("Core: ${CORE_FOUNDATION}")
find_library(IOKIT IOKit)
if (NOT IOKIT)
    message(FATAL_ERROR "IOKit not found")
endif()

add_executable(daum-ergoant src/main.cpp src/daumergo/DaumErgoPremium8i.cpp src/daumergo/DaumErgoPremium8i.h src/daumergo/DaumErgo.cpp
        src/daumergo/DaumErgo.h src/ant/AntService.cpp src/ant/AntService.h include/types.h
        src/daumergo/DaumErgo8008TRS.cpp src/daumergo/DaumErgo8008TRS.h src/utils/Serial.cpp src/utils/Serial.h
        src/ant/AntBicyclePower.cpp src/ant/AntBicyclePower.h src/ant/AntFECProfile.cpp src/ant/AntFECProfile.h src/ant/AntProfile.h src/ant/AntProfile.cpp)
target_link_libraries(daum-ergoant ${CMAKE_SOURCE_DIR}/lib/libantbase.a ${CORE_FOUNDATION} ${IOKIT})


