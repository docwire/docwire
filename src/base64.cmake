add_library(docwire_base64 SHARED base64.cpp)

find_library(base64 base64 REQUIRED)
target_link_libraries(docwire_base64 PRIVATE ${base64} docwire_core)
find_path(base64_inc_dir libbase64.h REQUIRED)
target_include_directories(docwire_base64 PRIVATE ${base64_inc_dir})

install(TARGETS docwire_base64 EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_base64> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_base64 EXPORT_FILE_NAME base64_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/base64_export.h DESTINATION include/docwire)
