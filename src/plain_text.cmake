add_library(df0_plain_text SHARED txt_parser.cpp)

find_library(charsetdetect charsetdetect REQUIRED)
target_link_libraries(df0_plain_text PRIVATE ${charsetdetect} df0_core)
find_path(charsetdetect_inc_dir charsetdetect.h REQUIRED)
target_include_directories(df0_plain_text PRIVATE ${charsetdetect_inc_dir})

install(TARGETS df0_plain_text EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_plain_text> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_plain_text EXPORT_FILE_NAME plain_text_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plain_text_export.h DESTINATION include/docwire)
