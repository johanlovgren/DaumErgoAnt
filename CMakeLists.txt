cmake_minimum_required(VERSION 3.17)
project(DaumErgoAnt)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/ant)
add_subdirectory(src/ant)

include_directories(src/daumergo)
add_subdirectory(src/daumergo)

include_directories(src/utils)
add_subdirectory(src/utils)

include_directories(${PROJECT_SOURCE_DIR}/src/libs/includes)

add_subdirectory(tests)
add_subdirectory(googletest)


find_library(CORE_FOUNDATION CoreFoundation)
if (NOT CORE_FOUNDATION)
    message(FATAL_ERROR "CoreFoundation not found")
endif()
message("Core: ${CORE_FOUNDATION}")
find_library(IOKIT IOKit)
if (NOT IOKIT)
    message(FATAL_ERROR "IOKit not found")
endif()

add_executable(daum-ergoant src/main.cpp)
target_link_libraries(daum-ergoant PUBLIC
        ant
        daumergo
        utils
        ${CMAKE_SOURCE_DIR}/lib/libantbase.a
        ${CORE_FOUNDATION}
        ${IOKIT})
