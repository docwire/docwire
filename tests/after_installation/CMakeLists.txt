cmake_minimum_required(VERSION 3.21)
project(docwire_after_installation_tests LANGUAGES CXX)

# This project is designed to be built and run AFTER the DocWire SDK has been installed.
# It simulates how a downstream user would find and use the SDK, verifying the installation.
# It relies on a CMake toolchain file or CMAKE_PREFIX_PATH to locate the installed DocWire SDK.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(WARNING "CMAKE_TOOLCHAIN_FILE is not defined. find_package(docwire) will rely on CMAKE_PREFIX_PATH to find the installed SDK.")
endif()

enable_testing()

# Verify that the DocWire SDK can be found in the current environment.
find_package(docwire CONFIG REQUIRED)

# --- 1. Integration Example Test ---
# This section rebuilds and tests the integration_example from scratch,
# relying on find_package(docwire) to link against the installed SDK.

# First, copy the example source files into a subdirectory of our build tree.
# The source files are located relative to this script in the main 'tests' directory.
set(EXAMPLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/integration_test_src/CMakeLists.txt
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/integration_test_src
    COMMAND ${CMAKE_COMMAND} -E copy ${EXAMPLE_SRC_DIR}/integration_example.cmake ${CMAKE_BINARY_DIR}/integration_test_src/CMakeLists.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${EXAMPLE_SRC_DIR}/integration_example.cpp ${CMAKE_BINARY_DIR}/integration_test_src/integration_example.cpp
    DEPENDS ${EXAMPLE_SRC_DIR}/integration_example.cmake ${EXAMPLE_SRC_DIR}/integration_example.cpp
    COMMENT "Setting up integration example source files"
)
add_custom_target(setup_integration_test ALL
    DEPENDS ${CMAKE_BINARY_DIR}/integration_test_src/CMakeLists.txt
)

# We must pass down the configuration that allows the sub-project to find dependencies.
# This is done by constructing a list of options to pass to the cmake command.
set(CONFIGURE_OPTIONS "")
if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "")
    list(APPEND CONFIGURE_OPTIONS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()
# The find_package(docwire) call above located the SDK. To ensure the nested
# integration_example project can also find it, we forward any relevant
# variables that were used for the initial find_package call.
if(DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    list(APPEND CONFIGURE_OPTIONS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif()
if(DEFINED CMAKE_PREFIX_PATH)
    list(APPEND CONFIGURE_OPTIONS "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
endif()
if(DEFINED VCPKG_TARGET_TRIPLET)
    list(APPEND CONFIGURE_OPTIONS "-DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")
endif()

# Test 1: Configure the integration example project
add_test(
    NAME integration_configure
    COMMAND ${CMAKE_COMMAND} ${CONFIGURE_OPTIONS}
        -S ${CMAKE_BINARY_DIR}/integration_test_src
        -B ${CMAKE_BINARY_DIR}/integration_test_build
)
set_property(TEST integration_configure PROPERTY DEPENDS setup_integration_test)

# Test 2: Build the integration example
add_test(
    NAME integration_build
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/integration_test_build --config $<CONFIG>
)
set_property(TEST integration_build PROPERTY DEPENDS integration_configure)

# Test 3: Run the built example from its build directory
# Determine where the executable is located in the build tree.
# Multi-config generators (like Visual Studio) use subdirectories (e.g., Debug/Release).
# Single-config generators (like Make/Ninja) place executables in the root of the build dir.
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    # For multi-config, the executable is in a config-specific subdirectory.
    # We use a generator expression to get the correct path at test time.
    set(integration_exe_path "${CMAKE_BINARY_DIR}/integration_test_build/$<CONFIG>/integration_example${CMAKE_EXECUTABLE_SUFFIX}")
    set(integration_working_dir "${CMAKE_BINARY_DIR}/integration_test_build/$<CONFIG>")
else()
    # For single-config, it's directly in the build directory.
    set(integration_exe_path "${CMAKE_BINARY_DIR}/integration_test_build/integration_example${CMAKE_EXECUTABLE_SUFFIX}")
    set(integration_working_dir "${CMAKE_BINARY_DIR}/integration_test_build")
endif()
add_test(
    NAME integration_run_from_build_tree
    COMMAND ${integration_exe_path} ${EXAMPLE_SRC_DIR}/data_processing_definition.doc
    WORKING_DIRECTORY ${integration_working_dir}
)
set_property(TEST integration_run_from_build_tree PROPERTY DEPENDS integration_build)

# Test 3.5: Verify the build type of the integration example executable
# We define per-configuration tests. CTest, when invoked with -C <config>,
# will automatically run the test whose CONFIGURATIONS list contains <config>.
# This works for both single- and multi-config generators.
add_test(
    NAME integration_verify_build_type_Release
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel
    COMMAND ${integration_exe_path} --check-build-type
)
set_property(TEST integration_verify_build_type_Release PROPERTY PASS_REGULAR_EXPRESSION "Build type: Release")
set_property(TEST integration_verify_build_type_Release PROPERTY DEPENDS integration_build)

add_test(
    NAME integration_verify_build_type_Debug
    CONFIGURATIONS Debug
    COMMAND ${integration_exe_path} --check-build-type
)
set_property(TEST integration_verify_build_type_Debug PROPERTY PASS_REGULAR_EXPRESSION "Build type: Debug")
set_property(TEST integration_verify_build_type_Debug PROPERTY DEPENDS integration_build)

# Test 4: Install the example (to test its install rules)
add_test(
    NAME integration_install
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}/integration_test_build --prefix ${CMAKE_BINARY_DIR}/integration_test_install --config $<CONFIG>
)
set_property(TEST integration_install PROPERTY DEPENDS integration_run_from_build_tree)

# Test 5: Run the installed example
add_test(
    NAME integration_run_installed
    COMMAND ${CMAKE_BINARY_DIR}/integration_test_install/bin/integration_example${CMAKE_EXECUTABLE_SUFFIX} ${EXAMPLE_SRC_DIR}/data_processing_definition.doc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/integration_test_install/bin
)
set_property(TEST integration_run_installed PROPERTY DEPENDS integration_install)


# --- 2. CLI Tests ---
# This section finds the 'docwire' executable in the path
# and runs it against test files.

find_program(DOCWIRE_CLI docwire REQUIRED)
message(STATUS "Found docwire CLI for testing at: ${DOCWIRE_CLI}")

foreach(file IN ITEMS 1.doc 1.pdf basic_ocr-eng.png)
    add_test(
        NAME cli_${file}
        COMMAND ${DOCWIRE_CLI} ${EXAMPLE_SRC_DIR}/${file}
    )
endforeach()