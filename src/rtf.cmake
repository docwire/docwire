add_library(df0_rtf SHARED rtf_parser.cpp)

target_link_libraries(df0_rtf PRIVATE df0_wv2 df0_core)

install(TARGETS df0_rtf EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_rtf> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_rtf EXPORT_FILE_NAME rtf_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtf_export.h DESTINATION include/docwire)
