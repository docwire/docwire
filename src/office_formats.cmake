add_library(docwire_office_formats SHARED ${EMPTY_SOURCE})

target_link_libraries(docwire_office_formats PUBLIC
    docwire_pdf
    docwire_odf_ooxml
    docwire_ole_office_formats
    docwire_xlsb
    docwire_iwork
    docwire_rtf
    docwire_html
    docwire_plain_text)

install(TARGETS docwire_office_formats)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()
