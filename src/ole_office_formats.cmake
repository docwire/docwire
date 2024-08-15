add_library(docwire_ole_office_formats SHARED
    oshared.cpp
    doc_parser.cpp
    ppt_parser.cpp
    xls_parser.cpp)

find_path(wv2_incdir wv2/ustring.h)
target_include_directories(docwire_ole_office_formats PRIVATE ${wv2_incdir})
find_library(wv2 wv2 HINTS ${wv2_incdir}/../lib/static REQUIRED)
target_link_libraries(docwire_ole_office_formats PRIVATE ${wv2} docwire_core)

install(TARGETS docwire_ole_office_formats)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_ole_office_formats> DESTINATION bin CONFIGURATIONS Debug)
endif()
