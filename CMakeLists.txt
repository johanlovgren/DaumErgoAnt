cmake_minimum_required(VERSION 3.17)
project(DaumErgoAnt)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/ant/)
include_directories(src/ergo)
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


add_executable(daum-ergoant main.cpp src/ergo/DaumErgoPremium8i.cpp src/ergo/DaumErgoPremium8i.h src/ergo/DaumErgo.cpp src/ergo/DaumErgo.h src/ant/AntService.cpp src/ant/AntService.h src/libs/includes/types.h src/ergo/DaumErgo8008TRS.cpp src/ergo/DaumErgo8008TRS.h src/utils/Serial.cpp src/utils/Serial.h)
target_link_libraries(daum-ergoant ${CMAKE_SOURCE_DIR}/src/libs/libantbase.a ${CORE_FOUNDATION} ${IOKIT})


