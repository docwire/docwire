add_library(docwire_openai SHARED
    analyze_data.cpp
    chat.cpp
    classify.cpp
    detect_sentiment.cpp
    extract_entities.cpp
    extract_keywords.cpp
    find.cpp
    summarize.cpp
    text_to_speech.cpp
    transcribe.cpp
    translate_to.cpp)

find_package(Boost REQUIRED COMPONENTS json)
target_link_libraries(docwire_openai PRIVATE docwire_base64 docwire_core Boost::json)

install(TARGETS docwire_openai EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_openai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_openai EXPORT_FILE_NAME openai_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/openai_export.h DESTINATION include/docwire)
