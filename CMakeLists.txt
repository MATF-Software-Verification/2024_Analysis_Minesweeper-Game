cmake_minimum_required(VERSION 3.20)
project(MinesweeperAnalysis CXX)

set(CMAKE_CXX_STANDARD 17)
set(CORE_DIR ${CMAKE_SOURCE_DIR}/external/Minesweeper-Game)

set(MS_CORE
    ${CORE_DIR}/Board.cpp
    ${CORE_DIR}/Cell.cpp
    ${CORE_DIR}/Minesweeper.cpp
    ${CORE_DIR}/Utility.cpp
)

add_library(ms_core ${MS_CORE})
target_include_directories(ms_core PUBLIC ${CORE_DIR})

add_executable(ms_app ${CORE_DIR}/Main.cpp)
target_link_libraries(ms_app PRIVATE ms_core)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Tests (Catch2)
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.3
)
FetchContent_MakeAvailable(catch2)

# Test executable
add_executable(ms_tests
    unit_tests/tests/test_board.cpp
    unit_tests/tests/test_gameplay.cpp
)

target_link_libraries(ms_tests PRIVATE ms_core Catch2::Catch2WithMain)
target_include_directories(ms_tests PRIVATE ${CORE_DIR})