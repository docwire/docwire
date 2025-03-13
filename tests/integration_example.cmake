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

if (WIN32 AND VCPKG_APPLOCAL_DEPS)
	# DLL files are copied to application directory on Windows (see VCPKG_APPLOCAL_DEPS) but resources are not
    file(COPY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/libmagic/misc/magic.mgc DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/share/libmagic/misc)
    file(COPY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/flan-t5-large-ct2-int8 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/share)
	file(COPY ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/tessdata-fast DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/share)
endif()
