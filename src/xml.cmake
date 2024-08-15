add_library(docwire_xml SHARED
    xml_stream.cpp
    xml_parser.cpp)

find_package(LibXml2 REQUIRED)
target_link_libraries(docwire_xml PRIVATE LibXml2::LibXml2 docwire_core)

install(TARGETS docwire_xml)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_xml> DESTINATION bin CONFIGURATIONS Debug)
endif()
