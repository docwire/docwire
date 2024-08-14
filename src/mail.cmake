add_library(docwire_mail SHARED eml_parser.cpp pst_parser.cpp)

find_library(bfio bfio REQUIRED)
find_library(pff pff REQUIRED)
find_library(htmlcxx htmlcxx REQUIRED)
find_package(mailio CONFIG REQUIRED)
target_link_libraries(docwire_mail PRIVATE ${bfio} ${pff} ${htmlcxx} mailio docwire_core)

install(TARGETS docwire_mail)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_mail> DESTINATION bin CONFIGURATIONS Debug)
endif()
