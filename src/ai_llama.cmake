
message(STATUS "DOCWIRE_LLAMA enabled: building llama backend")

add_library(docwire_ai_llama SHARED llama_runner.cpp)

find_package(llama CONFIG REQUIRED)

target_link_libraries(docwire_ai_llama PRIVATE docwire_core docwire_ai llama)

target_compile_definitions(docwire_ai_llama PUBLIC DOCWIRE_LLAMA)

docwire_find_resource(GRANITE_MODEL_FULL_PATH REL_PATH "granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf")
if(GRANITE_MODEL_FULL_PATH)
    docwire_target_resources(docwire_ai_llama "granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf" SOURCE "${GRANITE_MODEL_FULL_PATH}")
    docwire_deploy_resources(TARGETS docwire_ai_llama)
else()
    message(STATUS "Granite model not found (llama-granite feature not selected); model loading will be skipped at runtime.")
endif()

include(GenerateExportHeader)

generate_export_header(docwire_ai_llama EXPORT_FILE_NAME ai_llama_export.h)

target_include_directories(docwire_ai_llama PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/docwire>
)

install(TARGETS docwire_ai_llama EXPORT docwire_targets)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ai_llama_export.h
    DESTINATION include/docwire
)
