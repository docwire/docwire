add_executable(docwire docwire.cpp)

find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(docwire PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr docwire_archives
    docwire_local_ai docwire_openai docwire_ai docwire_content_type docwire_http Boost::program_options)

if(UNIX AND NOT APPLE)
    # On Linux, set the RPATH to find libraries in ../lib relative to the executable.
    set_property(TARGET docwire PROPERTY INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
elseif(APPLE)
    # On macOS, use @loader_path for the same purpose.
    set_property(TARGET docwire PROPERTY INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
endif()

install(TARGETS docwire DESTINATION bin)
