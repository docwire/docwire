add_library(df0_content_type SHARED
    content_type.cpp
    content_type_asp.cpp
    content_type_html.cpp
    content_type_image.cpp
    content_type_iwork.cpp
    content_type_odf_ooxml.cpp
    content_type_odf_flat.cpp
    content_type_outlook.cpp
    content_type_xlsb.cpp
    content_type_by_file_extension.cpp
    content_type_by_signature.cpp)

find_package(unofficial-libmagic REQUIRED)
target_link_libraries(df0_content_type PRIVATE unofficial::libmagic::libmagic df0_core)

install(TARGETS df0_content_type EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_content_type> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_content_type EXPORT_FILE_NAME content_type_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/content_type_export.h DESTINATION include/docwire)

df0_find_resource(MAGIC_MGC_FULL_PATH REL_PATH "libmagic/misc/magic.mgc" REQUIRED)
df0_target_resources(df0_content_type "libmagic/misc/magic.mgc" SOURCE "${MAGIC_MGC_FULL_PATH}")
