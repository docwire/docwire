set(EMPTY_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/office_formats_empty.cpp)
file(GENERATE OUTPUT ${EMPTY_SOURCE} CONTENT "
	#include \"office_formats_export.h\"
	namespace docwire
	{
		DOCWIRE_OFFICE_FORMATS_EXPORT void dummy() {}
	}
")

add_library(docwire_office_formats SHARED ${EMPTY_SOURCE})

target_link_libraries(docwire_office_formats PUBLIC
    docwire_pdf
    docwire_odf_ooxml
    docwire_ole_office_formats
    docwire_xlsb
    docwire_iwork
    docwire_rtf
    docwire_html
    docwire_xml
    docwire_plain_text)

install(TARGETS docwire_office_formats EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_office_formats EXPORT_FILE_NAME office_formats_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/office_formats_export.h DESTINATION include/docwire)
