add_library(docwire_detect SHARED
    content_type_html.cpp
    content_type_iwork.cpp
    content_type_odf_flat.cpp
    content_type_outlook.cpp
    content_type_xlsb.cpp
    detect_by_file_extension.cpp
    detect_by_signature.cpp)

target_compile_features(docwire_detect PUBLIC cxx_std_20)
if(MSVC)
    add_definitions(-DMSVC_BUILD)
    target_compile_options(docwire_detect PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

find_package(unofficial-libmagic REQUIRED)
target_link_libraries(docwire_detect PRIVATE unofficial::libmagic::libmagic)

install(TARGETS docwire_detect)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_detect> DESTINATION bin CONFIGURATIONS Debug)
endif()
