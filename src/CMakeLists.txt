set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_RPATH .)

add_library(docwire_core SHARED
        attachment.cpp
        chat.cpp
        extract_entities.cpp
        summarize.cpp
        translate_to.cpp
        exception.cpp
        decompress_archives.cpp
        list_style.cpp
        log.cpp
        metadata.cpp
        misc.cpp
        variant.cpp
        parser_parameters.cpp
        thread_safe_ole_storage.cpp
        thread_safe_ole_stream_reader.cpp
        data_stream.cpp
        parser.cpp
        parser_manager.cpp
        parser_parameters.cpp
        post.cpp
        html_writer.cpp
        csv_writer.cpp
        plain_text_writer.cpp
        standard_filter.cpp
        simple_extractor.cpp
        importer.cpp
        output.cpp
        plain_text_exporter.cpp
        html_exporter.cpp
        csv_exporter.cpp
        meta_data_exporter.cpp
        transformer_func.cpp
        meta_data_writer.cpp
        chain_element.cpp
        parsing_chain.cpp
        input.cpp)

find_package(Boost REQUIRED COMPONENTS filesystem system json program_options)
target_link_libraries(docwire_core PRIVATE Boost::filesystem Boost::system Boost::json)
target_include_directories(docwire_core PRIVATE ${Boost_INCLUDE_DIRS})
find_path(boost_signals2_inc boost/signals2.hpp HINT ${Boost_INCLUDE_DIRS} REQUIRED)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(docwire_core PRIVATE magic_enum::magic_enum)

find_package(unofficial-curlpp CONFIG REQUIRED)
target_link_libraries(docwire_core PRIVATE unofficial::curlpp::curlpp)

if(MSVC)
    add_definitions(-DMSVC_BUILD)
    find_path(wv2_inc_dir wv2/parser.h REQUIRED)
    set(wv2_prefix "${wv2_inc_dir}/..")
    find_library(wvWare wv2 HINTS "${wv2_prefix}/lib/static" REQUIRED)
    configure_file(${wv2_prefix}/bin/wv2.dll ${CMAKE_BINARY_DIR}/wv2.dll COPYONLY)
    find_package(PThreads4W REQUIRED)
    find_package(LibArchive REQUIRED)
    target_link_libraries(docwire_core PRIVATE ${wvWare}
                                                 PThreads4W::PThreads4W
                                                 LibArchive::LibArchive)
    target_include_directories(docwire_core PUBLIC ${wv2_inc_dir})
    target_compile_options(docwire_core PUBLIC /Zc:__cplusplus /Zc:preprocessor)

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
    find_package(LibArchive)
    if (LibArchive_FOUND)
        set(archive "LibArchive::LibArchive")
    else()
        find_library(archive archive REQUIRED)
    endif()
    target_link_libraries(docwire_core PRIVATE ${wvWare} pthread ${archive})
    target_include_directories(docwire_core PUBLIC ${wvWare_incdir})
endif()

target_compile_options(docwire_core PUBLIC ${EXTRA_FLAGS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(docwire_core PRIVATE dl)
endif()

add_library(docwire_parsers SHARED
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

target_include_directories(docwire_parsers PUBLIC
        ../
        .
        ${LIBXML2_INCLUDE_DIR}
        )

target_include_directories(docwire_core PUBLIC
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
    target_link_libraries(docwire_parsers PRIVATE
            ${podofo}
            ${unzip}
            ${wvWare}
            ${htmlcxx}
            ${LIBXML2_LIBRARIES}
            ${charsetdetect}
            docwire_core
            Freetype::Freetype
            PThreads4W::PThreads4W
            ZLIB::ZLIB
            Iconv::Iconv
            )
    target_compile_options(docwire_parsers PUBLIC /Zc:__cplusplus /Zc:preprocessor)
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
    target_link_libraries(docwire_parsers PRIVATE
        ${podofo}
        Freetype::Freetype
        ${unzip}
        ${wv2}
        ${htmlcxx}
        LibXml2::LibXml2 ${libxml2_dep_iconv} ${libxml2_dep_lzma}
        pthread
        ZLIB::ZLIB
        ${charsetdetect}
        docwire_core
        )
    find_path(unzip_incdir unzip.h)
    target_include_directories(docwire_parsers PRIVATE ${unzip_incdir})
endif()

add_library(docwire_ocr SHARED ocr_parser_provider.cpp ocr_parser.cpp)
if(MSVC)
    set_property(TARGET docwire_ocr PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    find_package(Leptonica)
    find_package(Tesseract)
    target_link_libraries(docwire_ocr ${Leptonica_LIBRARIES}
                                        Tesseract::libtesseract
                                        docwire_core)
    target_include_directories(docwire_ocr PUBLIC
            ../
            .
            )
    target_compile_options(docwire_ocr PUBLIC /Zc:__cplusplus /Zc:preprocessor)
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
    target_link_libraries(docwire_ocr PRIVATE ${Leptonica_LIBRARIES} ${tesseract} ${archive} ${libxml2_dep_iconv} docwire_core)
endif()

add_library(docwire_mail SHARED mail_parser_provider.cpp eml_parser.cpp pst_parser.cpp)
if(MSVC)
    find_library(bfio NAMES bfio libbfio REQUIRED)
    find_library(pff NAMES pff libpff REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    find_package(PThreads4W REQUIRED)
    target_link_libraries(docwire_mail PRIVATE
            ${bfio}
            ${pff}
            docwire_core
            ${htmlcxx}
            PThreads4W::PThreads4W)
    target_include_directories(docwire_mail PUBLIC
            ../
            .
            )
    target_compile_options(docwire_mail PUBLIC /Zc:__cplusplus /Zc:preprocessor)
else()
    find_library(bfio bfio REQUIRED)
    find_library(pff pff REQUIRED)
    find_library(htmlcxx htmlcxx REQUIRED)
    target_link_libraries(docwire_mail PRIVATE ${bfio} ${pff} ${htmlcxx} pthread docwire_core)
endif()
find_package(mailio CONFIG REQUIRED)
target_link_libraries(docwire_mail PRIVATE mailio)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   set_target_properties(docwire_parsers PROPERTIES PREFIX "")
   set_target_properties(docwire_core PROPERTIES PREFIX "")
endif()

add_executable(docwire docwire.cpp)
target_link_libraries(docwire PRIVATE docwire_core Boost::program_options)

if (DOCWIRE_TRACE)
   target_sources(docwire_core PRIVATE tracing)
   add_compile_definitions(TRACE)
   add_compile_options($<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-finstrument-functions>)
endif()

install(TARGETS docwire DESTINATION bin)
if (MSVC)
	set(plugins_dir bin/docwire_plugins)
else()
	set(plugins_dir lib/docwire_plugins)
endif()
install(TARGETS docwire_ocr LIBRARY DESTINATION ${plugins_dir} RUNTIME DESTINATION ${plugins_dir})
install(TARGETS docwire_mail LIBRARY DESTINATION ${plugins_dir} RUNTIME DESTINATION ${plugins_dir})
install(TARGETS docwire_parsers LIBRARY DESTINATION ${plugins_dir} RUNTIME DESTINATION ${plugins_dir})
install(TARGETS docwire_core)

if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_ocr> DESTINATION ${plugins_dir} CONFIGURATIONS Debug)
	install(FILES $<TARGET_PDB_FILE:docwire_mail> DESTINATION ${plugins_dir} CONFIGURATIONS Debug)
	install(FILES $<TARGET_PDB_FILE:docwire_parsers> DESTINATION ${plugins_dir} CONFIGURATIONS Debug)
	install(FILES $<TARGET_PDB_FILE:docwire_core> DESTINATION bin CONFIGURATIONS Debug)
endif()

set(PUBLIC_HEADERS
	chat.h
	extract_entities.h
	summarize.h
	translate_to.h
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
	post.h
	basic_parser_provider.h
	parser_provider.h
	standard_filter.h
	writer.h
	plain_text_writer.h
	html_writer.h
	csv_writer.h
	importer.h
	output.h
	plain_text_exporter.h
	html_exporter.h
	csv_exporter.h
	meta_data_exporter.h
	transformer_func.h
	parsing_chain.h
	simple_extractor.h
	defines.h
	log.h
)
install(FILES ${PUBLIC_HEADERS} DESTINATION include/docwire)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR APPLE)
	if(APPLE)
		file(WRITE ${CMAKE_BINARY_DIR}/docwire.sh "BASEDIR=$(dirname \"$0\")
			DYLD_FALLBACK_LIBRARY_PATH=$BASEDIR/../lib:$BASEDIR/../lib/docwire_system_libraries:$DYLD_FALLBACK_LIBRARY_PATH ./$BASEDIR/docwire \"$@\"")
	else()
		file(WRITE ${CMAKE_BINARY_DIR}/docwire.sh "BASEDIR=$(dirname \"$0\")
			LD_LIBRARY_PATH=$BASEDIR/../lib:$BASEDIR/../lib/docwire_system_libraries:$LD_LIBRARY_PATH ./$BASEDIR/docwire \"$@\"")
	endif()
    execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/docwire.sh)
	install(PROGRAMS ${CMAKE_BINARY_DIR}/docwire.sh DESTINATION bin)
elseif(MSVC)
	file(WRITE ${CMAKE_BINARY_DIR}/docwire.bat
		"@echo off
		set BASEDIR=%~dp0
		set PATH=%BASEDIR%\\..\\bin;%BASEDIR%\\..\\bin\\docwire_system_libraries;%PATH%
		\"%BASEDIR%\\docwire.exe\" %*")
	install(PROGRAMS ${CMAKE_BINARY_DIR}/docwire.bat DESTINATION bin)
endif()

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
	set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
endif()

include(InstallRequiredSystemLibraries)

if (WIN32)
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin/docwire_system_libraries)
else()
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION lib/docwire_system_libraries)
endif()
