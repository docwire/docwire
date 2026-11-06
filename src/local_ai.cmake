    add_library(df0_local_ai INTERFACE)
    target_link_libraries(df0_local_ai INTERFACE df0_ai)
    target_compile_definitions(df0_local_ai INTERFACE DF0_LOCAL_AI)
    target_compile_features(df0_local_ai INTERFACE cxx_std_20)
    target_include_directories(df0_local_ai
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    if(DF0_CT2)
    	target_link_libraries(df0_local_ai INTERFACE df0_ai_ct2)
        target_compile_definitions(df0_local_ai INTERFACE DF0_CT2)
    endif()
    if(DF0_LLAMA)
        target_link_libraries(df0_local_ai INTERFACE df0_ai_llama)
        target_compile_definitions(df0_local_ai INTERFACE DF0_LLAMA)
    endif()
    install(TARGETS df0_local_ai EXPORT df0_targets)
