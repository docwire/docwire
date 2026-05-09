
add_library(docwire_ai SHARED model_chain_element.cpp ai_summarize.cpp ai_translate.cpp ai_embed.cpp ai_task.cpp)

target_link_libraries(docwire_ai PUBLIC docwire_core)

target_compile_features(docwire_ai PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(docwire_ai PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

# Enable access to SDK headers
target_include_directories(docwire_ai PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # during building headers are in the source tree
    $<INSTALL_INTERFACE:include>) # after installation headers are in include/docwire

install(TARGETS docwire_ai EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_ai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_ai EXPORT_FILE_NAME ai_export.h)
target_include_directories(docwire_ai PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ai_export.h DESTINATION include/docwire)
