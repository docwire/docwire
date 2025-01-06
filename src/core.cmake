add_library(docwire_core SHARED
    chat.cpp
    data_source.cpp
    entities.cpp
    extract_entities.cpp
    summarize.cpp
    translate_to.cpp
    classify.cpp
    extract_keywords.cpp
    detect_sentiment.cpp
    analyze_data.cpp
    find.cpp
    text_to_speech.cpp
    transcribe.cpp
    decompress_archives.cpp
    log.cpp
    misc.cpp
    thread_safe_ole_storage.cpp
    thread_safe_ole_stream_reader.cpp
    data_stream.cpp
    parser.cpp
    post.cpp
    html_writer.cpp
    csv_writer.cpp
    plain_text_writer.cpp
    standard_filter.cpp
    output.cpp
    plain_text_exporter.cpp
    html_exporter.cpp
    csv_exporter.cpp
    meta_data_exporter.cpp
    transformer_func.cpp
    meta_data_writer.cpp
    chain_element.cpp
    parsing_chain.cpp
    resource_path.cpp
    zip_reader.cpp
    input.cpp)

target_compile_features(docwire_core PUBLIC cxx_std_20)
if(MSVC)
    add_definitions(-DMSVC_BUILD)
    target_compile_options(docwire_core PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

find_path(wv2_incdir wv2/ustring.h)

find_library(wv2 wv2 HINTS ${wv2_incdir}/../lib/static REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system json)
find_package(magic_enum CONFIG REQUIRED)
find_library(unzip unzip REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibArchive REQUIRED)
find_package(unofficial-curlpp CONFIG REQUIRED)
target_link_libraries(docwire_core PRIVATE
    ${wv2} Boost::filesystem Boost::system Boost::json magic_enum::magic_enum ${unzip}
    ZLIB::ZLIB LibArchive::LibArchive unofficial::curlpp::curlpp docwire_base64)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(docwire_core PRIVATE dl)
endif()

find_path(boost_signals2_inc boost/signals2.hpp HINT ${Boost_INCLUDE_DIRS} REQUIRED)
target_include_directories(docwire_core PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(docwire_core PUBLIC ../) # for version.h
target_include_directories(docwire_core PUBLIC .) # for other headers

install(TARGETS docwire_core)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_core> DESTINATION bin CONFIGURATIONS Debug)
endif()
