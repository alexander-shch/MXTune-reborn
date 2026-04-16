cmake_minimum_required(VERSION 4.0)
project(mxtune_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

add_executable(mxtune_tests
    test_midi_msg.cpp
    test_delay.cpp
    test_ring_buffer.cpp
    test_undo_redo.cpp
    test_auto_tune.cpp
    test_manual_tune.cpp
    ${SRC_DIR}/midi_msg.cpp
    ${SRC_DIR}/delay.cpp
    ${SRC_DIR}/ring_buffer.cpp
    ${SRC_DIR}/auto_tune.cpp
    ${SRC_DIR}/manual_tune.cpp
    ${SRC_DIR}/undo_redo.cpp
)

target_include_directories(mxtune_tests PRIVATE ${SRC_DIR})
target_link_libraries(mxtune_tests PRIVATE Catch2::Catch2WithMain)

enable_testing()
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(mxtune_tests)
