cmake_minimum_required(VERSION 3.10)
project(document_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set CMake policies
cmake_policy(SET CMP0135 NEW)
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

# Add nlohmann_json
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${nlohmann_json_SOURCE_DIR}/single_include)  
include_directories(${CMAKE_BINARY_DIR}/_deps/json-src/include)  

# Add source files explicitly
set(SOURCES
    src/server.cpp
    src/DocumentHandler.cpp
    src/ScheduleHandler.cpp
    src/GPACalculator.cpp
    src/GraduationProgress.cpp
    src/CoursePlanner.cpp
)

# Add executable
add_executable(server ${SOURCES})

# Link libraries
target_link_libraries(server
    PRIVATE
    Boost::system
    Threads::Threads
    nlohmann_json::nlohmann_json
)

# Include nlohmann/json
find_package(nlohmann_json 3.10.5 REQUIRED)

