
message(STATUS "DF0_LLAMA enabled: building llama backend")

add_library(df0_ai_llama SHARED llama_runner.cpp llama_models.cpp)

find_package(llama CONFIG REQUIRED)

target_link_libraries(df0_ai_llama PRIVATE df0_core df0_ai llama)

target_compile_definitions(df0_ai_llama PUBLIC DF0_LLAMA)

df0_find_resource(GRANITE_MODEL_FULL_PATH REL_PATH "granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf")
if(GRANITE_MODEL_FULL_PATH)
    df0_target_resources(df0_ai_llama "granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf" SOURCE "${GRANITE_MODEL_FULL_PATH}")
    df0_deploy_resources(TARGETS df0_ai_llama)
    target_compile_definitions(df0_ai_llama PUBLIC DF0_GRANITE)
else()
    message(STATUS "Granite model not found (llama-granite feature not selected); model loading will be skipped at runtime.")
endif()

include(GenerateExportHeader)

generate_export_header(df0_ai_llama EXPORT_FILE_NAME ai_llama_export.h)

target_include_directories(df0_ai_llama PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/df0>
)

install(TARGETS df0_ai_llama EXPORT df0_targets)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ai_llama_export.h
    DESTINATION include/df0
)
