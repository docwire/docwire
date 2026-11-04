add_library(df0_mail SHARED eml_parser.cpp pst_parser.cpp)

find_library(bfio bfio REQUIRED)
find_library(pff pff REQUIRED)
find_package(mailio CONFIG REQUIRED)
target_link_libraries(df0_mail PRIVATE ${bfio} ${pff} mailio::mailio df0_core)

install(TARGETS df0_mail EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_mail> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_mail EXPORT_FILE_NAME mail_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mail_export.h DESTINATION include/docwire)
