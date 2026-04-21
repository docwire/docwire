if(DOCWIRE_LLAMA)

    message(STATUS "DOCWIRE_LLAMA enabled: building llama backend")

    add_library(docwire_ai_llama SHARED llama_runner.cpp)

    find_package(llama CONFIG REQUIRED)

    target_link_libraries(docwire_ai_llama PRIVATE docwire_core docwire_ai llama)

    target_compile_definitions(docwire_ai_llama PUBLIC DOCWIRE_LLAMA)

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

endif()
