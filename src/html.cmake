add_library(docwire_html SHARED html_parser.cpp)

find_library(charsetdetect charsetdetect REQUIRED)
find_library(htmlcxx htmlcxx REQUIRED)
target_link_libraries(docwire_html PRIVATE ${charsetdetect} ${htmlcxx} docwire_core)
find_path(boost_algorithm_inc_dir boost/algorithm/string.hpp REQUIRED)
target_include_directories(docwire_html PRIVATE ${boost_algorithm_inc_dir})

install(TARGETS docwire_html)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_html> DESTINATION bin CONFIGURATIONS Debug)
endif()
