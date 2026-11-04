add_library(df0_iwork SHARED iwork_parser.cpp)

target_link_libraries(df0_iwork PRIVATE df0_core)

install(TARGETS df0_iwork EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_iwork> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_iwork EXPORT_FILE_NAME iwork_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/iwork_export.h DESTINATION include/df0)
