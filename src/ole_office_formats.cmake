add_library(docwire_ole_office_formats SHARED
    oshared.cpp
    doc_parser.cpp
    ppt_parser.cpp
    xls_parser.cpp)

target_link_libraries(docwire_ole_office_formats PRIVATE docwire_wv2 docwire_core)

install(TARGETS docwire_ole_office_formats EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_ole_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_ole_office_formats EXPORT_FILE_NAME ole_office_formats_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ole_office_formats_export.h DESTINATION include/docwire)
