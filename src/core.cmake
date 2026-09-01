add_library(docwire_core SHARED
    iconv_wrapper.cpp
    minizip_wrapper.cpp
    unique_identifier_globals.cpp
    error.cpp
    log_filter_globals.cpp
    log_cerr_redirection_globals.cpp
    log_sink_globals.cpp
    boost_datetime_wrapper.cpp
    boost_dll_wrapper.cpp
    boost_json_wrapper.cpp
    boost_demangle_wrapper.cpp)

target_compile_features(docwire_core PUBLIC cxx_std_20)
if(MSVC)
    add_definitions(-DMSVC_BUILD)
    target_compile_options(docwire_core PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

find_package(Boost REQUIRED COMPONENTS filesystem system json)
find_package(magic_enum CONFIG REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Iconv REQUIRED)
target_link_libraries(docwire_core PRIVATE
    Boost::filesystem Boost::system Boost::json magic_enum::magic_enum unofficial::minizip::minizip
    ZLIB::ZLIB Iconv::Iconv)
target_link_libraries(docwire_core PUBLIC magic_enum::magic_enum)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(docwire_core PRIVATE dl)
endif()

target_include_directories(docwire_core PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>) # for version.h

# Enable access to SDK headers
target_include_directories(docwire_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # during building headers are in the source tree
    $<INSTALL_INTERFACE:include>) # after installation headers are in include/docwire

install(TARGETS docwire_core EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_core> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_core EXPORT_FILE_NAME core_export.h)
target_include_directories(docwire_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/core_export.h DESTINATION include/docwire)
