cmake_minimum_required(VERSION 3.25)
project(cch-utils VERSION 0.0.1
                   DESCRIPTION "A set of C++ utility library"
                   LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
include(Dependencies.cmake)

# Main library target
# ------------------------------------------------------------------------------
add_library(cch-utils STATIC
    ${PROJECT_SOURCE_DIR}/src/date_time.cpp
    ${PROJECT_SOURCE_DIR}/src/fileio.cpp
    ${PROJECT_SOURCE_DIR}/src/filesystem.cpp
    ${PROJECT_SOURCE_DIR}/src/log.cpp
    ${PROJECT_SOURCE_DIR}/src/str.cpp
)

target_include_directories(cch-utils PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

# Test target
# ------------------------------------------------------------------------------
add_subdirectory(tests)
