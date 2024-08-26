add_library(docwire_odf_ooxml SHARED
    common_xml_document_parser.cpp
    odf_ooxml_parser.cpp
    odfxml_parser.cpp)

target_link_libraries(docwire_odf_ooxml PRIVATE docwire_xml docwire_core)
find_path(wv2_incdir wv2/ustring.h) # because of misc.h, TODO: misc.h should be splitted
target_include_directories(docwire_odf_ooxml PRIVATE ${wv2_incdir})

install(TARGETS docwire_odf_ooxml)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_odf_ooxml> DESTINATION bin CONFIGURATIONS Debug)
endif()
