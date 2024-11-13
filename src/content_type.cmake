add_library(docwire_content_type SHARED
    content_type_html.cpp
    content_type_iwork.cpp
    content_type_odf_flat.cpp
    content_type_outlook.cpp
    content_type_xlsb.cpp
    content_type_by_file_extension.cpp
    content_type_by_signature.cpp)

target_compile_features(docwire_content_type PUBLIC cxx_std_20)
if(MSVC)
    add_definitions(-DMSVC_BUILD)
    target_compile_options(docwire_content_type PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

find_package(unofficial-libmagic REQUIRED)
target_link_libraries(docwire_content_type PRIVATE unofficial::libmagic::libmagic)

install(TARGETS docwire_content_type)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_content_type> DESTINATION bin CONFIGURATIONS Debug)
endif()
