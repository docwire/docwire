add_executable(docwire docwire.cpp)

find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(docwire PRIVATE df0_core df0_office_formats df0_mail df0_ocr df0_archives
    df0_openai df0_ai df0_content_type df0_http Boost::program_options)

if(DF0_CT2 OR DF0_LLAMA)
    target_link_libraries(docwire PRIVATE df0_local_ai)
endif()

install(TARGETS docwire DESTINATION bin)
