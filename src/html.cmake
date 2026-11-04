add_library(df0_html SHARED html_parser.cpp html_writer.cpp html_exporter.cpp)

find_library(charsetdetect charsetdetect REQUIRED)
find_library(lexbor lexbor REQUIRED)
target_link_libraries(df0_html PRIVATE ${charsetdetect} ${lexbor} df0_core df0_base64)
find_path(boost_algorithm_inc_dir boost/algorithm/string.hpp REQUIRED)
target_include_directories(df0_html PRIVATE ${boost_algorithm_inc_dir})

install(TARGETS df0_html EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_html> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_html EXPORT_FILE_NAME html_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/html_export.h DESTINATION include/docwire)
