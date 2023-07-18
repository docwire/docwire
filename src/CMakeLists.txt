set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_RPATH .)

set(DOCTOTEXT_SOURCES
        doctotext_c_api.cpp
        attachment.cpp
        exception.cpp
        doctotext_link.cpp
        list_style.cpp
        metadata.cpp
        misc.cpp
        variant.cpp
        language.cpp
        parser_parameters.cpp
        thread_safe_ole_storage.cpp
        thread_safe_ole_stream_reader.cpp
        data_stream.cpp
        parser.cpp
        parser_manager.cpp
        parser_parameters.cpp
        html_writer.cpp
        csv_writer.cpp
        plain_text_writer.cpp
        standard_filter.cpp
        simple_extractor.cpp
        importer.cpp
        exporter.cpp
        transformer_func.cpp
        meta_data_writer.cpp
        chain_element.cpp
        parsing_chain.cpp
        input.cpp)

if(MSVC)
    add_definitions(-DMSVC_BUILD)
endif()

if (DOCTOTEXT_SHARED)
   add_library(doctotext_core SHARED ${DOCTOTEXT_SOURCES})
else()
   add_library(doctotext_core STATIC ${DOCTOTEXT_SOURCES})
endif()

if(MSVC)
    find_path(wv2_dll_path wv2.dll REQUIRED)
    configure_file(${wv2_dll_path}/wv2.dll ${CMAKE_BINARY_DIR}/wv2.dll COPYONLY)
    find_library(wvWare wv2 REQUIRED)
    #find_library(pthread_d pthreadVC3d HINTS "C:/vcpkg/packages/pthreads_x64-windows/debug/lib")
    #find_library(pthread pthreadVC3 HINTS "C:/vcpkg/packages/pthreads_x64-windows/lib")
    find_package(Boost REQUIRED COMPONENTS filesystem system)
    find_library(boost_filesystem boost_filesystem REQUIRED)
    find_library(boost_system boost_system REQUIRED)
    target_link_libraries(doctotext_core PRIVATE general ${wvWare}
                                                 #debug ${pthread_d}
                                                 #optimized ${pthread}
                                                 Boost::filesystem
                                                 Boost::system)

    set_target_properties( doctotext_core
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
            )
    target_compile_options(doctotext_core PUBLIC "/Zc:__cplusplus")

else()
    find_library(wvWare wv2 REQUIRED)
    get_filename_component(wvWare_libdir ${wvWare} DIRECTORY)
    set(wvWare_incdir "${wvWare_libdir}/../include")
    if(APPLE)
        set(wvWare_libname "libwv2.4.dylib")
    else()
        if(EXISTS "${wvWare_libdir}/libwv2.so.4")
            set(wvWare_libname "libwv2.so.4")
        else()
            set(wvWare_libname "libwv2.so.1")
        endif()
    endif()
    if (NOT MINGW)
        configure_file(${wvWare_libdir}/${wvWare_libname} ${CMAKE_BINARY_DIR}/src/${wvWare_libname} COPYONLY)
        configure_file(${wvWare_libdir}/${wvWare_libname} ${CMAKE_BINARY_DIR}/tests/${wvWare_libname} COPYONLY)
    endif()
    find_library(boost_filesystem boost_filesystem REQUIRED)
    find_library(boost_system boost_system REQUIRED)
    target_link_libraries(doctotext_core PRIVATE ${wvWare} pthread ${boost_filesystem} ${boost_system})
    find_path(boost_signals2_inc boost/signals2.hpp REQUIRED)
    target_include_directories(doctotext_core PUBLIC ${wvWare_incdir} ${boost_signals2_inc})
endif()

target_compile_options(doctotext_core PUBLIC ${EXTRA_FLAGS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(doctotext_core PRIVATE dl)
endif()

add_library(doctotext_parsers SHARED
        entities.cpp
        doctotext_unzip.cpp
        oshared.cpp
        xml_fixer.cpp
        xml_stream.cpp
        html_parser.cpp
        doc_parser.cpp
        common_xml_document_parser.cpp
        iwork_parser.cpp
        odf_ooxml_parser.cpp
        odfxml_parser.cpp
        pdf_parser.cpp
        ppt_parser.cpp
        rtf_parser.cpp
        txt_parser.cpp
        xls_parser.cpp
        xlsb_parser.cpp
        basic_parser_provider.cpp)

find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)

target_include_directories(doctotext_parsers PUBLIC
        ../
        .
        ${LIBXML2_INCLUDE_DIR}
        )

target_include_directories(doctotext_core PUBLIC
        ../
        .
        )
if(MSVC)
    target_link_libraries(doctotext_parsers PRIVATE
            general podofo
            general unzip
            general ${wvWare}
            general htmlcxx
            general ${LIBXML2_LIBRARIES}
            general charsetdetect
            general doctotext_core
            debug freetyped
            optimized freetype
            #debug ${pthread_d}
            #optimized ${pthread}
            debug zlibd
            optimized zlib
            debug libmimeticd
            optimized libmimetic
            )
    target_compile_options(doctotext_parsers PUBLIC "/Zc:__cplusplus")
else()
    find_library(podofo podofo REQUIRED)
    find_package(Freetype REQUIRED)
    find_library(unzip unzip REQUIRED)
    find_package(LibXml2 REQUIRED)
    if (NOT MINGW)
        find_package(Iconv REQUIRED)
        set(libxml2_dep_iconv "Iconv::Iconv")
        find_package(LibLZMA REQUIRED)
        set(libxml2_dep_lzma "LibLZMA::LibLZMA")
    endif()
    find_library(wv2 wv2 REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    find_library(charsetdetect charsetdetect REQUIRED)
    find_library(mimetic mimetic REQUIRED)
    target_link_libraries(doctotext_parsers PRIVATE
        ${podofo}
        Freetype::Freetype
        ${unzip}
        ${wv2}
        ${htmlcxx}
        LibXml2::LibXml2 ${libxml2_dep_iconv} ${libxml2_dep_lzma}
        pthread
        ZLIB::ZLIB
        ${charsetdetect}
        ${mimetic}
        doctotext_core
        )
    find_path(unzip_incdir unzip.h)
    target_include_directories(doctotext_parsers PRIVATE ${unzip_incdir})
endif()

add_library(doctotext_ocr SHARED ocr_parser_provider.cpp ocr_parser.cpp)
if(MSVC)
    set_property(TARGET doctotext_ocr PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_link_libraries(doctotext_ocr debug leptonica-1.82.0d
                                        debug tesseract53d
                                        optimized leptonica-1.82.0
                                        optimized tesseract53
                                        general doctotext_core)
    target_include_directories(doctotext_ocr PUBLIC
            ../
            .
            )
    target_compile_options(doctotext_ocr PUBLIC "/Zc:__cplusplus")
else()
    find_package(Leptonica)
    if (Leptonica_FOUND)
        find_package(LibArchive REQUIRED)
        set(archive "LibArchive::LibArchive")
    else ()
        if (MINGW)
            set(Leptonica_LIBRARIES "lept")
        else()
            find_library(Leptonica_LIBRARIES leptonica REQUIRED)
        endif()
        set(archive "")
    endif()
    find_package(Tesseract)
    if (Tesseract_FOUND)
        set(tesseract Tesseract::libtesseract)
    else()
        find_library(tesseract tesseract)
    endif()
    if (APPLE)
        find_package(Iconv REQUIRED)
        set(libxml2_dep_iconv "Iconv::Iconv")
    else()
        set(libxml2_dep_iconv "")
    endif()
    target_link_libraries(doctotext_ocr PRIVATE ${Leptonica_LIBRARIES} ${tesseract} ${archive} ${libxml2_dep_iconv} doctotext_core)
endif()

add_library(doctotext_mail SHARED mail_parser_provider.cpp eml_parser.cpp pst_parser.cpp)
if(MSVC)
    target_link_libraries(doctotext_mail
            general libbfio
            general libpff
            general doctotext_core
            general htmlcxx
            debug libmimeticd
            optimized libmimetic
            #debug ${pthread_d}
            #optimized ${pthread})
    target_include_directories(doctotext_mail PUBLIC
            ../
            .
            )
    target_compile_options(doctotext_mail PUBLIC "/Zc:__cplusplus")
else()
    find_library(mimetic mimetic REQUIRED)
    find_library(bfio bfio REQUIRED)
    find_library(pff pff REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    target_link_libraries(doctotext_mail PRIVATE ${bfio} ${pff} ${mimetic} ${htmlcxx} pthread doctotext_core)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   target_link_libraries(doctotext_parsers PRIVATE
      iconv
   )
   set_target_properties(doctotext_parsers PROPERTIES PREFIX "")
   set_target_properties(doctotext_core PROPERTIES PREFIX "")
endif()

add_executable(c_text_extractor c_text_extractor.c)
target_link_libraries(c_text_extractor PRIVATE doctotext_core)

add_executable(text_extractor text_extractor.cpp)
target_link_libraries(text_extractor PRIVATE doctotext_core)

if (DOCTOTEXT_TRACE)
   target_sources(doctotext_core PRIVATE tracing)
   add_compile_definitions(TRACE)
   add_compile_options($<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-finstrument-functions>)
endif()

install(TARGETS c_text_extractor
   DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS text_extractor
   DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS doctotext_ocr
        DESTINATION ${CMAKE_BINARY_DIR}/plugins)
install(TARGETS doctotext_mail
        DESTINATION ${CMAKE_BINARY_DIR}/plugins)
install(TARGETS doctotext_parsers
        DESTINATION ${CMAKE_BINARY_DIR}/plugins)
install(TARGETS doctotext_core
        DESTINATION ${CMAKE_BINARY_DIR})

INSTALL(FILES ${CMAKE_BINARY_DIR}/src/Debug/doctotext_ocr.pdb ${CMAKE_BINARY_DIR}/src/Debug/doctotext_parsers.pdb
        DESTINATION  ${CMAKE_BINARY_DIR}/plugins
        CONFIGURATIONS Debug
        )

configure_file(${CMAKE_SOURCE_DIR}/src/doctotext_c_api.h ${CMAKE_BINARY_DIR}/doctotext_c_api.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/metadata.h ${CMAKE_BINARY_DIR}/metadata.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/formatting_style.h ${CMAKE_BINARY_DIR}/formatting_style.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/doctotext_link.h ${CMAKE_BINARY_DIR}/doctotext_link.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/exception.h ${CMAKE_BINARY_DIR}/exception.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/attachment.h ${CMAKE_BINARY_DIR}/attachment.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/variant.h ${CMAKE_BINARY_DIR}/variant.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/parser.h ${CMAKE_BINARY_DIR}/parser.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/parser_manager.h ${CMAKE_BINARY_DIR}/parser_manager.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/parser_builder.h ${CMAKE_BINARY_DIR}/parser_builder.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/parser_parameters.h ${CMAKE_BINARY_DIR}/parser_parameters.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/basic_parser_provider.h ${CMAKE_BINARY_DIR}/basic_parser_provider.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/parser_provider.h ${CMAKE_BINARY_DIR}/parser_provider.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/standard_filter.h ${CMAKE_BINARY_DIR}/standard_filter.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/writer.h ${CMAKE_BINARY_DIR}/writer.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/plain_text_writer.h ${CMAKE_BINARY_DIR}/plain_text_writer.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/html_writer.h ${CMAKE_BINARY_DIR}/html_writer.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/csv_writer.h ${CMAKE_BINARY_DIR}/csv_writer.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/importer.h ${CMAKE_BINARY_DIR}/importer.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/exporter.h ${CMAKE_BINARY_DIR}/exporter.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/transformer_func.h ${CMAKE_BINARY_DIR}/transformer_func.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/parsing_chain.h ${CMAKE_BINARY_DIR}/parsing_chain.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/simple_extractor.h ${CMAKE_BINARY_DIR}/simple_extractor.h COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/defines.h ${CMAKE_BINARY_DIR}/defines.h COPYONLY)


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR APPLE)
    file(WRITE ${CMAKE_BINARY_DIR}/text_extractor.sh "BASEDIR=$(dirname \"$0\")
    LD_LIBRARY_PATH=$BASEDIR:$$LD_LIBRARY_PATH ./$BASEDIR/text_extractor \"$@\"")
    execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/text_extractor.sh)

   file(WRITE ${CMAKE_BINARY_DIR}/c_text_extractor.sh "BASEDIR=$(dirname \"$0\")
   LD_LIBRARY_PATH=$BASEDIR:$$LD_LIBRARY_PATH ./$BASEDIR/c_text_extractor \"$@\"")
   execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/c_text_extractor.sh)
endif()
