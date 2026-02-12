cmake_minimum_required(VERSION 3.15)
project(integration_example LANGUAGES CXX)

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

find_package(docwire CONFIG REQUIRED)
# Choose SDK libraries that you need to use
target_link_libraries(integration_example PRIVATE docwire_core docwire_content_type docwire_office_formats docwire_mail)

# Automatically deploy resources (creates .path files pointing to installed resources)
docwire_deploy_resources(integration_example)
