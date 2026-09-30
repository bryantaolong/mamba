cmake_minimum_required(VERSION 3.14)
project(mamba CXX)

# Required early: CMAKE_INSTALL_INCLUDEDIR/LIBDIR are used in
# target_include_directories and install.cmake
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library
add_library(mamba_lib
    src/command.cpp
    src/mamba.cpp
)

target_include_directories(mamba_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Integration test / example binary
add_executable(mamba_tests
    tests/main.cpp
)

target_link_libraries(mamba_tests PRIVATE mamba_lib)

# Unit tests
enable_testing()

add_executable(test_command
    tests/test_command.cpp
)

target_link_libraries(test_command PRIVATE mamba_lib)

add_test(NAME CommandTests COMMAND test_command)

# Install targets (only when this is the top-level project, not when used via add_subdirectory)
option(MAMBA_INSTALL "Install headers and library" ON)
if(MAMBA_INSTALL AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake")
endif()
