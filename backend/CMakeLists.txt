cmake_minimum_required(VERSION 3.10)
project(WebSocketServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

# Add Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests/mocks
)

# Add server executable
add_executable(server 
    src/server.cpp
    src/DocumentHandler.cpp
    src/ScheduleHandler.cpp
    src/GPACalculator.cpp
    src/GraduationProgress.cpp
    src/CoursePlanner.cpp
)

target_link_libraries(server 
    PRIVATE 
    Boost::system
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Add test executable
add_executable(unit_tests
    tests/test_main.cpp
    tests/test_schedulerhandler.cpp
    tests/test_gpacalculator.cpp
    tests/test_courseplanner.cpp
    tests/test_graduationprogress.cpp
    tests/test_documenthandler.cpp
    src/DocumentHandler.cpp
    src/ScheduleHandler.cpp
    src/GPACalculator.cpp
    src/CoursePlanner.cpp
    src/GraduationProgress.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    GTest::gtest_main
    GTest::gmock_main
    Boost::system
    nlohmann_json::nlohmann_json
    Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

