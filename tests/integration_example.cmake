cmake_minimum_required(VERSION 3.21)
project(integration_example LANGUAGES CXX)


add_executable(integration_example integration_example.cpp)

# Find the DocWire SDK package.
# This requires CMAKE_TOOLCHAIN_FILE to be set (e.g. for vcpkg) or CMAKE_PREFIX_PATH to point to the installation.
find_package(docwire CONFIG REQUIRED)

# DocWire SDK is designed to be used as a shared library (DLL/SO/DYLIB).
# Static linking is not supported due to the complexity of dependencies and resource management.
# This check verifies that the found 'docwire_core' target is a shared library.
get_target_property(DOCWIRE_CORE_TYPE docwire_core TYPE)
if(NOT DOCWIRE_CORE_TYPE STREQUAL "SHARED_LIBRARY")
    message(FATAL_ERROR "DocWire SDK requires dynamic linking, but the found 'docwire_core' target is not a shared library. Please ensure you are using a dynamic build configuration.")
endif()

# Link against the specific DocWire libraries your application needs.
# docwire_core is always required. Other modules are optional based on usage.
target_link_libraries(integration_example PRIVATE docwire_core docwire_content_type docwire_office_formats docwire_local_ai)

include(GNUInstallDirs)

# --- RPATH Configuration ---
# On Linux and macOS, executables do not automatically look in their own directory for shared libraries.
# We must configure the RPATH (Run-Time Search Path) to include the directory where we install the libraries.
# We install libraries to 'lib' (CMAKE_INSTALL_LIBDIR) and the executable to 'bin' (CMAKE_INSTALL_BINDIR).
if(UNIX AND NOT APPLE)
    set_property(TARGET integration_example PROPERTY INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
elseif(APPLE)
    set_property(TARGET integration_example PROPERTY INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
endif()

# --- Build Tree Configuration ---
# This function ensures that DocWire resources (like OCR models, dictionaries, etc.)
# are accessible to your application when running directly from the build directory.
# It creates .path files that point to the actual location of resources in the vcpkg installed tree.
docwire_deploy_resources(TARGETS integration_example)

# --- Installation Configuration ---
# The following section configures 'cmake --install' to create a standalone distribution
# of your application, including all necessary DocWire runtime dependencies.

# 1. Install the application executable and its runtime dependencies.
install(TARGETS integration_example
    # 2. Install runtime dependencies (DLLs on Windows, SOs on Linux, DYLIBs on macOS).
    # CMake's RUNTIME_DEPENDENCIES automatically finds all transitive shared library dependencies.
    RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES
            # Exclude Windows API Sets (part of the OS, not physical files to distribute)
            "api-ms-.*"
            "ext-ms-.*"
            # Exclude CI/Cloud environment specific DLLs (transitive system dependencies found on Azure/GitHub Actions runners)
            "azureattest.*"
            "hvsifiletrust.*"
            "pdmutilities.*"
            "wtdsensor.*"
            "wpaxholder.*"
            "wtdccm.*"
            # Exclude common Windows system libraries and Visual C++ Runtime (which should be installed via Redistributable)
            "ucrtbase.*"
            "vcruntime.*"
            "msvcp.*"
    # On non-Windows platforms, we must specify where to install library and framework dependencies
    # because executables don't have default LIBRARY or FRAMEWORK destinations to inherit from.
    # Note: System frameworks (in /System/Library) are automatically excluded by CMake, so this
    # destination is only used for 3rd-party frameworks (e.g. from vcpkg) if any are found.
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    FRAMEWORK DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

# 3. Install DocWire data resources.
# This function installs the necessary data files (e.g., tessdata for OCR, magic database)
# into the appropriate 'share' directory in your installation prefix.
docwire_install_resources(TARGETS integration_example)
