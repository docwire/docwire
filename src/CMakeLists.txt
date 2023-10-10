set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_RPATH .)

set(DOCTOTEXT_SOURCES
        attachment.cpp
        exception.cpp
        decompress_archives.cpp
        list_style.cpp
        log.cpp
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
    find_path(wv2_inc_dir wv2/parser.h REQUIRED)
    set(wv2_prefix "${wv2_inc_dir}/..")
    find_library(wvWare wv2 HINTS "${wv2_prefix}/lib/static" REQUIRED)
    configure_file(${wv2_prefix}/bin/wv2.dll ${CMAKE_BINARY_DIR}/wv2.dll COPYONLY)
    find_package(PThreads4W REQUIRED)
    find_package(Boost REQUIRED COMPONENTS filesystem system json)
    find_package(LibArchive REQUIRED)
    target_link_libraries(doctotext_core PRIVATE ${wvWare}
                                                 PThreads4W::PThreads4W
                                                 Boost::filesystem
                                                 Boost::system
                                                 Boost::json
                                                 LibArchive::LibArchive)
    target_include_directories(doctotext_core PUBLIC ${wv2_inc_dir})
    target_compile_options(doctotext_core PUBLIC /Zc:__cplusplus /Zc:preprocessor)

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
    find_library(boost_json boost_json REQUIRED)
    find_package(LibArchive)
    if (LibArchive_FOUND)
        set(archive "LibArchive::LibArchive")
    else()
        find_library(archive archive REQUIRED)
    endif()
    target_link_libraries(doctotext_core PRIVATE ${wvWare} pthread ${boost_filesystem} ${boost_system} ${boost_json} ${archive})
    find_path(boost_signals2_inc boost/signals2.hpp REQUIRED)
    target_include_directories(doctotext_core PUBLIC ${wvWare_incdir} ${boost_signals2_inc})
endif()

target_compile_options(doctotext_core PUBLIC ${EXTRA_FLAGS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(doctotext_core PRIVATE dl)
endif()

add_library(doctotext_parsers SHARED
        entities.cpp
        zip_reader.cpp
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
    find_library(podofo podofo REQUIRED)
    find_package(Freetype REQUIRED)
    find_library(unzip unzip REQUIRED)
    find_package(PThreads4W REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    find_library(charsetdetect charsetdetect REQUIRED)
    find_package(Iconv REQUIRED)
    target_link_libraries(doctotext_parsers PRIVATE
            ${podofo}
            ${unzip}
            ${wvWare}
            ${htmlcxx}
            ${LIBXML2_LIBRARIES}
            ${charsetdetect}
            doctotext_core
            Freetype::Freetype
            PThreads4W::PThreads4W
            ZLIB::ZLIB
            Iconv::Iconv
            )
    target_compile_options(doctotext_parsers PUBLIC /Zc:__cplusplus /Zc:preprocessor)
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
        doctotext_core
        )
    find_path(unzip_incdir unzip.h)
    target_include_directories(doctotext_parsers PRIVATE ${unzip_incdir})
endif()

add_library(doctotext_ocr SHARED ocr_parser_provider.cpp ocr_parser.cpp)
if(MSVC)
    set_property(TARGET doctotext_ocr PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    find_package(Leptonica)
    find_package(Tesseract)
    target_link_libraries(doctotext_ocr ${Leptonica_LIBRARIES}
                                        Tesseract::libtesseract
                                        doctotext_core)
    target_include_directories(doctotext_ocr PUBLIC
            ../
            .
            )
    target_compile_options(doctotext_ocr PUBLIC /Zc:__cplusplus /Zc:preprocessor)
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
    find_library(bfio NAMES bfio libbfio REQUIRED)
    find_library(pff NAMES pff libpff REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    find_package(PThreads4W REQUIRED)
    target_link_libraries(doctotext_mail PRIVATE
            ${bfio}
            ${pff}
            doctotext_core
            ${htmlcxx}
            PThreads4W::PThreads4W)
    target_include_directories(doctotext_mail PUBLIC
            ../
            .
            )
    target_compile_options(doctotext_mail PUBLIC /Zc:__cplusplus /Zc:preprocessor)
else()
    find_library(bfio bfio REQUIRED)
    find_library(pff pff REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    target_link_libraries(doctotext_mail PRIVATE ${bfio} ${pff} ${htmlcxx} pthread doctotext_core)
endif()
find_package(mailio CONFIG REQUIRED)
target_link_libraries(doctotext_mail PRIVATE mailio)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   set_target_properties(doctotext_parsers PROPERTIES PREFIX "")
   set_target_properties(doctotext_core PROPERTIES PREFIX "")
endif()

add_executable(text_extractor text_extractor.cpp)
target_link_libraries(text_extractor PRIVATE doctotext_core)

if (DOCTOTEXT_TRACE)
   target_sources(doctotext_core PRIVATE tracing)
   add_compile_definitions(TRACE)
   add_compile_options($<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-finstrument-functions>)
endif()

install(TARGETS text_extractor DESTINATION bin)
if (MSVC)
	set(plugins_dir bin/doctotext_plugins)
else()
	set(plugins_dir lib/doctotext_plugins)
endif()
install(TARGETS doctotext_ocr LIBRARY DESTINATION ${plugins_dir} RUNTIME DESTINATION ${plugins_dir})
install(TARGETS doctotext_mail LIBRARY DESTINATION ${plugins_dir} RUNTIME DESTINATION ${plugins_dir})
install(TARGETS doctotext_parsers LIBRARY DESTINATION ${plugins_dir} RUNTIME DESTINATION ${plugins_dir})
install(TARGETS doctotext_core)

if(MSVC)
	install(FILES $<TARGET_PDB_FILE:doctotext_ocr> DESTINATION ${plugins_dir} CONFIGURATIONS Debug)
	install(FILES $<TARGET_PDB_FILE:doctotext_mail> DESTINATION ${plugins_dir} CONFIGURATIONS Debug)
	install(FILES $<TARGET_PDB_FILE:doctotext_parsers> DESTINATION ${plugins_dir} CONFIGURATIONS Debug)
	install(FILES $<TARGET_PDB_FILE:doctotext_core> DESTINATION bin CONFIGURATIONS Debug)
endif()

set(PUBLIC_HEADERS
	metadata.h
	formatting_style.h
	decompress_archives.h
	exception.h
	attachment.h
	variant.h
	parser.h
	parser_manager.h
	parser_builder.h
	parser_parameters.h
	basic_parser_provider.h
	parser_provider.h
	standard_filter.h
	writer.h
	plain_text_writer.h
	html_writer.h
	csv_writer.h
	importer.h
	exporter.h
	transformer_func.h
	parsing_chain.h
	simple_extractor.h
	defines.h
	log.h
)
install(FILES ${PUBLIC_HEADERS} DESTINATION include/doctotext)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR APPLE)
	if(APPLE)
		file(WRITE ${CMAKE_BINARY_DIR}/text_extractor.sh "BASEDIR=$(dirname \"$0\")
			DYLD_FALLBACK_LIBRARY_PATH=$BASEDIR/../lib:$BASEDIR/../lib/doctotext_system_libraries:$DYLD_FALLBACK_LIBRARY_PATH ./$BASEDIR/text_extractor \"$@\"")
	else()
    file(WRITE ${CMAKE_BINARY_DIR}/text_extractor.sh "BASEDIR=$(dirname \"$0\")
    LD_LIBRARY_PATH=$BASEDIR/../lib:$BASEDIR/../lib/doctotext_system_libraries:$LD_LIBRARY_PATH ./$BASEDIR/text_extractor \"$@\"")
	endif()
    execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/text_extractor.sh)
	install(PROGRAMS ${CMAKE_BINARY_DIR}/text_extractor.sh DESTINATION bin)
elseif(MSVC)
	file(WRITE ${CMAKE_BINARY_DIR}/text_extractor.bat
		"@echo off
		set BASEDIR=%~dp0
		set PATH=%BASEDIR%\\..\\bin;%BASEDIR%\\..\\bin\\doctotext_system_libraries;%PATH%
		\"%BASEDIR%\\text_extractor.exe\" %*")
	install(PROGRAMS ${CMAKE_BINARY_DIR}/text_extractor.bat DESTINATION bin)
endif()

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
	set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
endif()

include(InstallRequiredSystemLibraries)

if (WIN32)
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin/doctotext_system_libraries)
else()
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION lib/doctotext_system_libraries)
endif()
