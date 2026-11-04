add_library(df0_odf_ooxml SHARED
    common_xml_document_parser.cpp
    odf_ooxml_parser.cpp
    odfxml_parser.cpp)

target_link_libraries(df0_odf_ooxml PRIVATE
    df0_xml
    df0_core
    df0_ole_utils)

install(TARGETS df0_odf_ooxml EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_odf_ooxml> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_odf_ooxml EXPORT_FILE_NAME odf_ooxml_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/odf_ooxml_export.h DESTINATION include/df0)
