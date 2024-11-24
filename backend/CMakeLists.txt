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
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files explicitly
set(SOURCES
    src/server.cpp
    src/DocumentHandler.cpp
    src/ScheduleHandler.cpp
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
