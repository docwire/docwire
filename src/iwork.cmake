add_library(docwire_iwork SHARED iwork_parser.cpp)

target_link_libraries(docwire_iwork PRIVATE docwire_core)

install(TARGETS docwire_iwork EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_iwork> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_iwork EXPORT_FILE_NAME iwork_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/iwork_export.h DESTINATION include/docwire)
