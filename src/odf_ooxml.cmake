add_library(docwire_odf_ooxml SHARED
    common_xml_document_parser.cpp
    odf_ooxml_parser.cpp
    odfxml_parser.cpp)

target_link_libraries(docwire_odf_ooxml PRIVATE docwire_xml docwire_core)

install(TARGETS docwire_odf_ooxml EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_odf_ooxml> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_odf_ooxml EXPORT_FILE_NAME odf_ooxml_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/odf_ooxml_export.h DESTINATION include/docwire)
