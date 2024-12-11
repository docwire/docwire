add_library(docwire_content_type SHARED
    content_type_html.cpp
    content_type_iwork.cpp
    content_type_odf_flat.cpp
    content_type_outlook.cpp
    content_type_xlsb.cpp
    content_type_by_file_extension.cpp
    content_type_by_signature.cpp)

find_package(unofficial-libmagic REQUIRED)
target_link_libraries(docwire_content_type PRIVATE unofficial::libmagic::libmagic docwire_core)

install(TARGETS docwire_content_type)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_content_type> DESTINATION bin CONFIGURATIONS Debug)
endif()
