add_library(df0_http SHARED
    http_server.cpp
    post.cpp
    ssl_certificate.cpp
    http_listener.cpp
)

find_package(httplib CONFIG REQUIRED COMPONENTS OpenSSL)

target_link_libraries(df0_http PRIVATE httplib::httplib df0_core df0_content_type)

install(TARGETS df0_http EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_http> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_http EXPORT_FILE_NAME http_export.h)
target_include_directories(df0_http PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/http_export.h DESTINATION include/df0)
