add_library(docwire_office_formats SHARED
    entities.cpp
    zip_reader.cpp
    oshared.cpp
    xml_fixer.cpp
    xml_stream.cpp
    html_parser.cpp
    doc_parser.cpp
    common_xml_document_parser.cpp
    iwork_parser.cpp
    odf_ooxml_parser.cpp
    odfxml_parser.cpp
    pdf_parser.cpp
    ppt_parser.cpp
    rtf_parser.cpp
    txt_parser.cpp
    xls_parser.cpp
    xlsb_parser.cpp
    office_formats_parser_provider.cpp)

find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Freetype REQUIRED)
find_library(unzip unzip REQUIRED)
find_library(htmlcxx htmlcxx REQUIRED)
find_library(charsetdetect charsetdetect REQUIRED)
find_package(Iconv REQUIRED)
find_library(podofo podofo REQUIRED)
find_package(LibLZMA REQUIRED)
find_library(wv2 wv2 REQUIRED)
target_link_libraries(docwire_office_formats PRIVATE ${podofo} ${wv2} ${htmlcxx} LibXml2::LibXml2 ${unzip}
    ${charsetdetect} Freetype::Freetype ZLIB::ZLIB Iconv::Iconv LibLZMA::LibLZMA docwire_core)

install(TARGETS docwire_office_formats)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()
