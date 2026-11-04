add_library(df0_base64 SHARED base64.cpp)

find_library(base64 base64 REQUIRED)
target_link_libraries(df0_base64 PRIVATE ${base64} df0_core)
find_path(base64_inc_dir libbase64.h REQUIRED)
target_include_directories(df0_base64 PRIVATE ${base64_inc_dir})

install(TARGETS df0_base64 EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_base64> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_base64 EXPORT_FILE_NAME base64_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/base64_export.h DESTINATION include/df0)
