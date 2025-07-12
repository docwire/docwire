add_executable(docwire docwire.cpp)

find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(docwire PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr docwire_archives
    docwire_local_ai docwire_openai docwire_content_type docwire_http Boost::program_options)

install(TARGETS docwire DESTINATION bin)
