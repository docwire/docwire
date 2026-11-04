add_library(df0_ole_office_formats SHARED
    doc_parser.cpp
    ppt_parser.cpp
    xls_parser.cpp)

target_link_libraries(df0_ole_office_formats PRIVATE
    df0_wv2
    df0_core
    df0_ole_utils
    df0_audit)

install(TARGETS df0_ole_office_formats EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_ole_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_ole_office_formats EXPORT_FILE_NAME ole_office_formats_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ole_office_formats_export.h DESTINATION include/docwire)
