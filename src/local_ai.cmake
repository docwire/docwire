    add_library(docwire_local_ai INTERFACE)
    target_link_libraries(docwire_local_ai INTERFACE docwire_ai)
    target_compile_definitions(docwire_local_ai INTERFACE DOCWIRE_LOCAL_AI)
    target_compile_features(docwire_local_ai INTERFACE cxx_std_20)
    target_include_directories(docwire_local_ai
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    if(DOCWIRE_CT2)
    	target_link_libraries(docwire_local_ai INTERFACE docwire_ai_ct2)
        target_compile_definitions(docwire_local_ai INTERFACE DOCWIRE_CT2)
    endif()
    if(DOCWIRE_LLAMA)
        target_link_libraries(docwire_local_ai INTERFACE docwire_ai_llama)
        target_compile_definitions(docwire_local_ai INTERFACE DOCWIRE_LLAMA)
    endif()
    install(TARGETS docwire_local_ai EXPORT docwire_targets)
