add_executable(docwire docwire.cpp)

find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(docwire PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr docwire_archives
    docwire_openai docwire_ai docwire_content_type docwire_http Boost::program_options)

if(DOCWIRE_LOCAL_CT2 OR DOCWIRE_LLAMA)
    target_link_libraries(docwire PRIVATE docwire_local_ai)
endif()

install(TARGETS docwire DESTINATION bin)
