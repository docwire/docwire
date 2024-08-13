add_library(docwire_rtf SHARED rtf_parser.cpp)

target_link_libraries(docwire_rtf PRIVATE docwire_core)
find_path(wv2_incdir wv2/ustring.h) # because of misc.h, TODO: misc.h should be splitted
target_include_directories(docwire_rtf PRIVATE ${wv2_incdir})

install(TARGETS docwire_rtf)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_rtf> DESTINATION bin CONFIGURATIONS Debug)
endif()
