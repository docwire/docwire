set(EMPTY_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/office_formats_empty.cpp)
file(GENERATE OUTPUT ${EMPTY_SOURCE} CONTENT "
	#include \"office_formats_export.h\"
	namespace df0
	{
		DF0_OFFICE_FORMATS_EXPORT void dummy() {}
	}
")

add_library(df0_office_formats SHARED ${EMPTY_SOURCE})

target_link_libraries(df0_office_formats PUBLIC
    df0_pdf
    df0_odf_ooxml
    df0_ole_office_formats
    df0_xlsb
    df0_iwork
    df0_rtf
    df0_html
    df0_xml
    df0_plain_text)

install(TARGETS df0_office_formats EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_office_formats EXPORT_FILE_NAME office_formats_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/office_formats_export.h DESTINATION include/df0)
