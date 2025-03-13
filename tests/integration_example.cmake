cmake_minimum_required(VERSION 3.15)

# On Linux and macOS the default VCPKG triplets use static linking but DocWire SDK requires dynamic linking
if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set(VCPKG_TARGET_TRIPLET "x64-linux-dynamic")
elseif(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm64")
        set(VCPKG_TARGET_TRIPLET "arm64-osx-dynamic")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-osx-dynamic")
    endif()
endif()

project(integration_example LANGUAGES CXX)
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
