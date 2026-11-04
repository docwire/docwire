add_library(df0_core SHARED
    iconv_wrapper.cpp
    minizip_wrapper.cpp
    unique_identifier_globals.cpp
    error.cpp
    boost_datetime_wrapper.cpp
    boost_dll_wrapper.cpp
    boost_json_wrapper.cpp
    boost_demangle_wrapper.cpp)

target_compile_features(df0_core PUBLIC cxx_std_20)
if(MSVC)
    add_definitions(-DMSVC_BUILD)
    target_compile_options(df0_core PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

find_package(Boost REQUIRED COMPONENTS filesystem system json)
find_package(magic_enum CONFIG REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Iconv REQUIRED)
target_link_libraries(df0_core PRIVATE
    Boost::filesystem Boost::system Boost::json magic_enum::magic_enum unofficial::minizip::minizip
    ZLIB::ZLIB Iconv::Iconv)
target_link_libraries(df0_core PUBLIC magic_enum::magic_enum)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(df0_core PRIVATE dl)
endif()

target_include_directories(df0_core PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>) # for version.h

# Enable access to SDK headers
target_include_directories(df0_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # during building headers are in the source tree
    $<INSTALL_INTERFACE:include>) # after installation headers are in include/df0

install(TARGETS df0_core EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_core> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_core EXPORT_FILE_NAME core_export.h)
target_include_directories(df0_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/core_export.h DESTINATION include/df0)
