add_library(docwire_xlsb SHARED xlsb_parser.cpp)

target_link_libraries(docwire_xlsb PRIVATE docwire_core)
find_path(wv2_incdir wv2/ustring.h) # because of misc.h, TODO: misc.h should be splitted
target_include_directories(docwire_xlsb PRIVATE ${wv2_incdir})

install(TARGETS docwire_xlsb)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_xlsb> DESTINATION bin CONFIGURATIONS Debug)
endif()