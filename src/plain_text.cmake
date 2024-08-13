add_library(docwire_plain_text SHARED txt_parser.cpp)

find_library(charsetdetect charsetdetect REQUIRED)
target_link_libraries(docwire_plain_text PRIVATE ${charsetdetect} docwire_core)
find_path(charsetdetect_inc_dir charsetdetect.h REQUIRED)
target_include_directories(docwire_plain_text PRIVATE ${charsetdetect_inc_dir})

install(TARGETS docwire_plain_text)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_plain_text> DESTINATION bin CONFIGURATIONS Debug)
endif()
