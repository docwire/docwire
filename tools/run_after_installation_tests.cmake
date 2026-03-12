cmake_minimum_required(VERSION 3.17)

# This script configures and runs the "after_installation" tests.
# It is designed to be run from the root of the repository after the
# main SDK has been built and installed into the local vcpkg instance.
#
# It provides a simple, cross-platform way for developers to run these
# tests locally, mirroring the CI process.
#
# Usage:
#   cmake -P tools/run_after_installation_tests.cmake
#   (Triplet will be auto-detected based on host OS)
#   (Build type defaults to Release)
#
# Example with explicit triplet and build type:
#   cmake -DTRIPLET=x64-windows -DBUILD_TYPE=Debug -P tools/run_after_installation_tests.cmake

if(NOT DEFINED TRIPLET)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(TRIPLET "x64-windows")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(TRIPLET "arm64-osx-dynamic")
        else()
            set(TRIPLET "x64-osx-dynamic")
        endif()
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(TRIPLET "x64-linux-dynamic")
    endif()

    if(DEFINED TRIPLET)
        message(STATUS "Auto-detected vcpkg triplet: ${TRIPLET}")
    endif()
endif()

if(NOT DEFINED TRIPLET)
    message(FATAL_ERROR "The vcpkg triplet could not be auto-detected. Please provide it via -DTRIPLET=<your-triplet>.")
endif()

if(NOT DEFINED BUILD_TYPE)
    set(BUILD_TYPE "Release")
endif()

set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../tests/after_installation")
set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/../tests/after_installation/build-${BUILD_TYPE}")

if(NOT DEFINED TOOLCHAIN_FILE)
    set(TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

# --- 1. Argument Preparation ---
set(CONFIGURE_ARGS
    "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"
    "-DVCPKG_TARGET_TRIPLET=${TRIPLET}"
    "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
)

# --- 2. Configure Step ---
message(STATUS "Configuring after-installation tests...")
execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${SOURCE_DIR} -B ${BUILD_DIR} ${CONFIGURE_ARGS}
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_output
)

if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "Failed to configure after-installation tests:\n${configure_output}")
endif()
message(STATUS "Configuration successful.")

# --- 3. Build Step ---
# Although ctest can trigger a build, an explicit build step makes the script's flow clearer.
message(STATUS "Building after-installation tests...")
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR} --config ${BUILD_TYPE}
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_output
    ERROR_VARIABLE build_output
)
if(NOT build_result EQUAL 0)
    message(FATAL_ERROR "Failed to build after-installation tests:\n${build_output}")
endif()
message(STATUS "Build successful.")

# --- 4. Test Step ---
message(STATUS "Running after-installation tests...")
execute_process(
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${BUILD_DIR} --output-on-failure -C ${BUILD_TYPE}
    RESULT_VARIABLE test_result
)

if(NOT test_result EQUAL 0)
    message(FATAL_ERROR "After-installation tests failed.")
endif()

message(STATUS "Removing build directory: ${BUILD_DIR}")
file(REMOVE_RECURSE ${BUILD_DIR})

message(STATUS "After-installation tests passed successfully.")
