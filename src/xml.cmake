add_library(df0_xml SHARED
    xml_fixer.cpp
    xml_reader.cpp
    xml_parser.cpp)

find_package(LibXml2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS algorithm)
target_include_directories(df0_xml PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(df0_xml PRIVATE LibXml2::LibXml2 df0_core)

install(TARGETS df0_xml EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_xml> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_xml EXPORT_FILE_NAME xml_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xml_export.h DESTINATION include/docwire)
