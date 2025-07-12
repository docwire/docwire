add_library(docwire_http SHARED
    post.cpp
)

find_package(unofficial-curlpp CONFIG REQUIRED)
target_link_libraries(docwire_http PRIVATE docwire_core unofficial::curlpp::curlpp)

install(TARGETS docwire_http EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_http> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_http EXPORT_FILE_NAME http_export.h)
target_include_directories(docwire_http PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/http_export.h DESTINATION include/docwire)
