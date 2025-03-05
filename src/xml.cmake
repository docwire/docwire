add_library(docwire_xml SHARED
    xml_fixer.cpp
    xml_stream.cpp
    xml_parser.cpp)

find_package(LibXml2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS algorithm)
target_include_directories(docwire_xml PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(docwire_xml PRIVATE LibXml2::LibXml2 docwire_core)

install(TARGETS docwire_xml EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_xml> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_xml EXPORT_FILE_NAME xml_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xml_export.h DESTINATION include/docwire)
