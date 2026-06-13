if(DOCWIRE_LOCAL_CT2)
    add_library(docwire_local_ai SHARED
        local_ai_summarize.cpp
        local_ai_embed.cpp
        local_ai_translate.cpp
        local_ai_task.cpp
    )
    target_link_libraries(docwire_local_ai PUBLIC docwire_ai docwire_ai_ct2)
    target_compile_definitions(docwire_local_ai PUBLIC DOCWIRE_LOCAL_AI DOCWIRE_LOCAL_CT2)
    target_compile_features(docwire_local_ai PUBLIC cxx_std_20)
    target_include_directories(docwire_local_ai
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    include(GenerateExportHeader)
    generate_export_header(docwire_local_ai EXPORT_FILE_NAME local_ai_export.h)
    target_include_directories(docwire_local_ai
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/local_ai_export.h
        DESTINATION include/docwire)
    if(DOCWIRE_LLAMA)
        target_link_libraries(docwire_local_ai PUBLIC docwire_ai_llama)
        target_compile_definitions(docwire_local_ai PUBLIC DOCWIRE_LLAMA)
    endif()
    install(TARGETS docwire_local_ai EXPORT docwire_targets)
    if(MSVC)
        install(FILES $<TARGET_PDB_FILE:docwire_local_ai>
            DESTINATION bin CONFIGURATIONS Debug)
    endif()
else()
    add_library(docwire_local_ai INTERFACE)
    target_link_libraries(docwire_local_ai INTERFACE docwire_ai)
    target_compile_definitions(docwire_local_ai INTERFACE DOCWIRE_LOCAL_AI)
    target_compile_features(docwire_local_ai INTERFACE cxx_std_20)
    target_include_directories(docwire_local_ai
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    if(DOCWIRE_LLAMA)
        target_link_libraries(docwire_local_ai INTERFACE docwire_ai_llama)
        target_compile_definitions(docwire_local_ai INTERFACE DOCWIRE_LLAMA)
    endif()
    install(TARGETS docwire_local_ai EXPORT docwire_targets)
endif()
