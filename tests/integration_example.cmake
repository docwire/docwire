cmake_minimum_required(VERSION 3.21)
project(integration_example LANGUAGES CXX)


# DocWire SDK is designed to be used as a shared library (DLL/SO/DYLIB).
# Static linking is not supported due to the complexity of dependencies and resource management.
# Ensure that vcpkg is configured to use a dynamic triplet.
if (VCPKG_TARGET_TRIPLET STREQUAL "x64-linux")
    message(FATAL_ERROR "DocWire SDK requires dynamic linking. Use triplet x64-linux-dynamic instead of x64-linux")
endif()
if (VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static")
    message(FATAL_ERROR "DocWire SDK requires dynamic linking. Use triplet x64-windows instead of x64-windows-static")
endif()
if (VCPKG_TARGET_TRIPLET STREQUAL "x64-osx")
    message(FATAL_ERROR "DocWire SDK requires dynamic linking. Use triplet x64-osx-dynamic instead of x64-osx")
endif()
if (VCPKG_TARGET_TRIPLET STREQUAL "arm64-osx")
    message(FATAL_ERROR "DocWire SDK requires dynamic linking. Use triplet arm64-osx-dynamic instead of arm64-osx")
endif()

add_executable(integration_example integration_example.cpp)

# Find the DocWire SDK package.
# This requires CMAKE_TOOLCHAIN_FILE to be set to the vcpkg toolchain file.
find_package(docwire CONFIG REQUIRED)

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
        POST_EXCLUDE_REGEXES
            # Exclude common Windows system libraries and Visual C++ Runtime (which should be installed via Redistributable)
            "api-ms-win-.*\\.dll"
            "ext-ms-win-.*\\.dll"
            "ucrtbase\\.dll"
            "vcruntime.*\\.dll"
            "msvcp.*\\.dll"
            # On Linux/macOS, you might want to exclude libc, libstdc++, etc., depending on your deployment strategy.
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
