
add_library(docwire_local_ai SHARED
    local_ai_summarize.cpp
    local_ai_embed.cpp
    local_ai_translate.cpp
    local_ai_misc_task.cpp
)
target_link_libraries(docwire_local_ai PUBLIC docwire_ai)
target_compile_definitions(docwire_local_ai PUBLIC DOCWIRE_LOCAL_AI)

if(DOCWIRE_LLAMA)
    target_link_libraries(docwire_local_ai PUBLIC docwire_ai_llama)
    target_compile_definitions(docwire_local_ai PUBLIC DOCWIRE_LLAMA)
endif()

if(DOCWIRE_LOCAL_CT2)
    target_link_libraries(docwire_local_ai PUBLIC docwire_ai_ct2)
    target_compile_definitions(docwire_local_ai PUBLIC DOCWIRE_LOCAL_CT2)
endif()

target_include_directories(docwire_local_ai
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(docwire_local_ai PUBLIC cxx_std_20)


include(GenerateExportHeader)

generate_export_header(docwire_local_ai EXPORT_FILE_NAME local_ai_export.h)

target_include_directories(docwire_local_ai PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/local_ai_export.h
    DESTINATION include/docwire
)

install(TARGETS docwire_local_ai EXPORT docwire_targets)

if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_local_ai>
        DESTINATION bin CONFIGURATIONS Debug)
endif()
