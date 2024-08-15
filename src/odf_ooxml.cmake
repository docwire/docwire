add_library(docwire_odf_ooxml SHARED
    xml_fixer.cpp
    xml_stream.cpp
    common_xml_document_parser.cpp
    odf_ooxml_parser.cpp
    odfxml_parser.cpp)

find_package(LibXml2 REQUIRED)
find_path(wv2_incdir wv2/ustring.h) # because of misc.h, TODO: misc.h should be splitted
target_link_libraries(docwire_odf_ooxml PRIVATE LibXml2::LibXml2 docwire_core)
target_include_directories(docwire_odf_ooxml PRIVATE ${wv2_incdir})

install(TARGETS docwire_odf_ooxml)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_odf_ooxml> DESTINATION bin CONFIGURATIONS Debug)
endif()
