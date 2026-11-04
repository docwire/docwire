
add_library(df0_ai SHARED model_chain_element.cpp ai_summarize.cpp ai_translate.cpp ai_embed.cpp ai_task.cpp ai_find.cpp ai_detect_sentiment.cpp)

target_link_libraries(df0_ai PUBLIC df0_core)

target_compile_features(df0_ai PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(df0_ai PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

# Enable access to SDK headers
target_include_directories(df0_ai PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # during building headers are in the source tree
    $<INSTALL_INTERFACE:include>) # after installation headers are in include/docwire

install(TARGETS df0_ai EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_ai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_ai EXPORT_FILE_NAME ai_export.h)
target_include_directories(df0_ai PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ai_export.h DESTINATION include/docwire)
