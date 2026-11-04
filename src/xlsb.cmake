add_library(df0_xlsb SHARED xlsb_parser.cpp)

target_link_libraries(df0_xlsb PRIVATE
    df0_core
    df0_ole_utils)

install(TARGETS df0_xlsb EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_xlsb> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_xlsb EXPORT_FILE_NAME xlsb_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xlsb_export.h DESTINATION include/df0)
